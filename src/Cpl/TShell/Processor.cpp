/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Processor.h"
#include "Cpl/System/GlobalLock.h"
#include "Cpl/Text/strip.h"
#include "Cpl/System/Assert.h"

#ifdef USE_CPL_TSHELL_PROCESSOR_SILENT_WHEN_PUBLIC
#define OUTPUT_PROMPT()                     if ( m_userPermLevel != Security::ePUBLIC ) { outfd.write( OPTION_CPL_TSHELL_PROCESSOR_PROMPT ); }
#define OUTPUT_GREETING()                   if ( m_userPermLevel != Security::ePUBLIC ) { outfd.write( OPTION_CPL_TSHELL_PROCESSOR_GREETING ); }
#define OUTPUT_FAREWELL()                   if ( m_userPermLevel != Security::ePUBLIC ) { outfd.write( OPTION_CPL_TSHELL_PROCESSOR_FAREWELL ); }
#define OUTPUT_CMD_ERROR_AND_RETURN()       if ( m_userPermLevel != Security::ePUBLIC ) \
                                            { \
                                                return outputCommandError( Command::eERROR_INVALID_CMD, deframedInput ) ? Command::eERROR_INVALID_CMD : Command::eERROR_IO; \
                                            } \
                                            else \
                                            { \
                                                return Command::eERROR_INVALID_CMD; \
                                            }

#else
#define OUTPUT_PROMPT()                     outfd.write( OPTION_CPL_TSHELL_PROCESSOR_PROMPT )
#define OUTPUT_GREETING()                   outfd.write( OPTION_CPL_TSHELL_PROCESSOR_GREETING )
#define OUTPUT_FAREWELL()                   outfd.write( OPTION_CPL_TSHELL_PROCESSOR_FAREWELL)
#define OUTPUT_CMD_ERROR_AND_RETURN()       return outputCommandError( Command::eERROR_INVALID_CMD, deframedInput ) ? Command::eERROR_INVALID_CMD : Command::eERROR_IO;       
#endif


///
using namespace Cpl::TShell;

static const char* resultToString_( Command::Result_T errcode );


///////////////////////////////////
Processor::Processor( Cpl::Container::Map<Command>&commands,
                      Cpl::Text::Frame::StreamDecoder & deframer,
                      Cpl::Text::Frame::StreamEncoder & framer,
                      Cpl::System::Mutex & outputLock,
                      char                              commentChar,
                      char                              argEscape,
                      char                              argDelimiter,
                      char                              argQuote,
                      char                              argTerminator,
                      Security::Permission_T            initialPermissionLevel
)
    :m_commands( commands )
    , m_deframer( deframer )
    , m_framer( framer )
    , m_outLock( outputLock )
    , m_userPermLevel( initialPermissionLevel )
    , m_comment( commentChar )
    , m_esc( argEscape )
    , m_del( argDelimiter )
    , m_quote( argQuote )
    , m_term( argTerminator )
    , m_running( false )
{
}

///////////////////////////////////
Security::Permission_T Processor::getUserPermissionLevel() const noexcept
{
    return m_userPermLevel;
}

/// See Cpl::TShell::Context_
Security::Permission_T Processor::setUserPermissionLevel( Security::Permission_T newPermissionLevel ) noexcept
{
    Security::Permission_T oldLevel = m_userPermLevel;
    m_userPermLevel = newPermissionLevel;
    return oldLevel;
}

int Processor::readInput( size_t & frameSize ) noexcept
{
    frameSize = 0;
    if ( !m_deframer.scan( OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE, m_inputBuffer, frameSize ) )
    {
        // Error reading raw input -->exit the Command processor
        return -1;
    }

    return 1;
}

int Processor::getAndProcessFrame( Cpl::Io::Output & outfd ) noexcept
{
    // Check for stop request
    Cpl::System::GlobalLock::begin();
    bool run = m_running;
    Cpl::System::GlobalLock::end();

    if ( !run )
    {
        OUTPUT_FAREWELL();
        Cpl::System::Api::sleep( 250 ); // Allow time for the farewell message to be outputted
        return 1;
    }


    // Get the next command string from my input stream
    size_t frameSize;
    int readResult = readInput( frameSize );
    if ( readResult < 0 )
    {
        // Input stream error 
        return -1;
    }
    else if ( readResult == 1 )
    {
        // Ensure the raw input buffer gets null terminated
        m_inputBuffer[frameSize] = '\0';

        // Execute the command
        Command::Result_T result = executeCommand( m_inputBuffer, outfd );
        if ( result == Command::eERROR_IO )
        {
            // Output stream error 
            return -1;
        }

        // Output the prompt
        OUTPUT_PROMPT();
    }

    // If I get here - then no error occurred
    return 0;
}

///////////////////////////////////
bool Processor::start( Cpl::Io::Input & infd, Cpl::Io::Output & outfd, bool blocking ) noexcept
{
    // Housekeeping
    m_running = true;
    m_outputBuffer.clear();
    m_framer.setOutput( outfd );

    // Output the greeting message
    OUTPUT_GREETING();
    OUTPUT_PROMPT();
    m_deframer.setInput( infd );

    // Run until I am requested to stop (or run once if using non-blocking semantics)
    do
    {
        int result = getAndProcessFrame( outfd );
        if ( result != 0 )
        {
            return result >= 0 ? true : false;
        }
    } while ( blocking );


    // If I get here, then the command processor self terminated
    return false;
}


void Processor::requestStop() noexcept
{
    Cpl::System::GlobalLock::begin();
    m_running = false;
    Cpl::System::GlobalLock::end();
}

int Processor::poll() noexcept
{
    // NOT SUPPORTED!
    CPL_SYSTEM_ASSERT( false );
    return -1;
}


///////////////////////////////////
Command::Result_T Processor::executeCommand( char* deframedInput, Cpl::Io::Output & outfd ) noexcept
{
    const char* firstToken = Cpl::Text::stripSpace( deframedInput );

    // Skip blank and comment lines
    if ( *firstToken == 0 || *firstToken == m_comment )
    {
        return Command::eSUCCESS;
    }

    // Lookup the command to be executed
    const char* endFirstToken = Cpl::Text::stripNotSpace( firstToken );
    Cpl::Container::KeyStringBuffer verb( firstToken, endFirstToken - firstToken );
    Command*                         cmdPtr;
    if ( (cmdPtr=m_commands.find( verb )) == 0 )
    {
        OUTPUT_CMD_ERROR_AND_RETURN();
    }

    // Validate the user permissions
    Command::Result_T result = Command::eERROR_INVALID_CMD;
    if ( cmdPtr->getMinPermissionRequired() <= m_userPermLevel )
    {
        // Execute the found command
        result = cmdPtr->execute( *this, (char*) firstToken, outfd );
        if ( result != Command::eSUCCESS )
        {
            return outputCommandError( result, deframedInput ) ? result : Command::eERROR_IO;
        }
    }

    return result;
}

///////////////////////////////////
Cpl::Container::Map<Command>& Processor::getCommands() noexcept
{
    return m_commands;
}

bool Processor::writeFrame( const char* text ) noexcept
{
    // Encode and output the text
    bool io = true;
    m_outLock.lock();
    io &= m_framer.startFrame();
    io &= m_framer.output( text );
    io &= m_framer.endFrame();
    m_outLock.unlock();

    return io;
}

bool Processor::writeFrame( const char* text, size_t maxBytes ) noexcept
{
    // Encode and output the text
    bool io = true;
    m_outLock.lock();
    io &= m_framer.startFrame();
    io &= m_framer.output( text, maxBytes );
    io &= m_framer.endFrame();
    m_outLock.unlock();

    return io;
}

bool Processor::oobRead( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    return m_deframer.oobRead( buffer, numBytes, bytesRead );
}


///////////////////////////////////
Cpl::Text::String& Processor::getOutputBuffer() noexcept
{
    return m_outputBuffer;
}

Cpl::Text::String& Processor::getTokenBuffer() noexcept
{
    m_tokenBuffer.clear();
    return m_tokenBuffer;
}

Cpl::Text::String& Processor::getTokenBuffer2() noexcept
{
    m_tokenBuffer2.clear();
    return m_tokenBuffer2;
}

char Processor::getEscapeChar() noexcept
{
    return m_esc;
}

char Processor::getDelimiterChar() noexcept
{
    return m_del;
}

char Processor::getQuoteChar() noexcept
{
    return m_quote;
}

char Processor::getTerminatorChar() noexcept
{
    return m_term;
}

const char* resultToString_( Command::Result_T errcode )
{
    switch ( errcode )
    {
    default:
        return "ERRNO ?: Command failed - unknown error code";

    case Command::eERROR_BAD_SYNTAX:
        return "ERRNO 1: Unable to parse command string";

    case Command::eERROR_INVALID_CMD:
        return "ERRNO 2: Command not supported";

    case Command::eERROR_IO:
        return "ERRNO 3: Input/Output stream IO encounter";

    case Command::eERROR_MISSING_ARGS:
        return "ERRNO 4: Command is missing argument(s)";

    case Command::eERROR_EXTRA_ARGS:
        return "ERRNO 5: Command encounter 'extra' argument(s)";

    case Command::eERROR_INVALID_ARGS:
        return "ERRNO 6: One or more Command arguments are incorrect/invalid";

    case Command::eERROR_FAILED:
        return "ERRNO 7: Command failed to complete one or more of its actions";
    }

    return "ERROR: I SHOULD NEVER GET HERE!";
}

bool Processor::outputCommandError( Command::Result_T result, const char* deframedInput ) noexcept
{
    bool io = true;

    m_outputBuffer.format( "ERROR: [%s]", deframedInput );
    io &= writeFrame( m_outputBuffer );
    io &= writeFrame( resultToString_( result ) );

    return io;
}