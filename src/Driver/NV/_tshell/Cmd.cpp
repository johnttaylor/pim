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

#include "Cmd.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"
#include <string.h>         

///
using namespace Driver::NV;


///////////////////////////
Cmd::Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
          Driver::NV::Api&                           nvDriver,
          Cpl::TShell::Security::Permission_T        minPermLevel ) noexcept
    : Command( commandList, verb, minPermLevel )
    , m_nvDriver( nvDriver )
{
}

///////////////////////////
Cpl::TShell::Command::Result_T Cmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    bool                            io       = true;

    // ERASE
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "ERASE" ) == 0 )
    {
        // Get work buffer (and set the 'erased' value)
        int      bufLen;
        uint8_t* buf = (uint8_t*) context.getTokenBuffer().getBuffer( bufLen );
        memset( buf, 0xFF, bufLen );

        // Erase the entire NV storage
        unsigned      bytesRemaining = m_nvDriver.getTotalSize();
        unsigned      offset         = 0;
        unsigned long startTime      = Cpl::System::ElapsedTime::milliseconds();
        while ( bytesRemaining )
        {
            size_t bytesToWrite = bytesRemaining > (unsigned) bufLen ? (unsigned) bufLen : bytesRemaining;
            if ( !m_nvDriver.write( offset, buf, bytesToWrite ) )
            {
                outtext.format( "Erase failed at offset: %lu", offset );
                context.writeFrame( outtext );
                return Command::eERROR_FAILED;
            }
            bytesRemaining -= bytesToWrite;
            offset         += bytesToWrite;
        }

        unsigned long endTime = Cpl::System::ElapsedTime::milliseconds();
        outtext.format( "Erase completed. Duration = %lu ms", endTime - startTime );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Read
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'r' )
    {
        unsigned offset = 0;
        if ( !Cpl::Text::a2ui( offset, tokens.getParameter( 2 ) ) )
        {
            outtext.format( "Invalid 'offset': %s", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }
        unsigned numBytes = 0;
        if ( !Cpl::Text::a2ui( numBytes, tokens.getParameter( 3 ) ) )
        {
            outtext.format( "Invalid 'len': %s", tokens.getParameter( 3 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Get a temporary buffer for the binary data (and make sure it is not too big to convert to ASCII HEX)
        int   bufLen;
        char* tmpBuf = context.getTokenBuffer().getBuffer( bufLen );
        int   maxBufLen = outtext.maxLength() / 3;
        if ( bufLen > maxBufLen )
        {
            bufLen = maxBufLen;
        }

        // Read the request bytes (in chunks)
        while ( numBytes )
        {
            unsigned bytesToRead = numBytes > (unsigned) bufLen ? (unsigned) bufLen : numBytes;
            if ( !m_nvDriver.read( offset, tmpBuf, bytesToRead ) )
            {
                outtext.format( "Read failed.  Offset=%u, len=%u", offset, bytesToRead );
                io &= context.writeFrame( outtext );
                return Command::eERROR_FAILED;
            }
            Cpl::Text::bufferToAsciiHex( tmpBuf, bytesToRead, outtext, true, false, ' ' );
            io &= context.writeFrame( outtext );

            numBytes -= bytesToRead;
            offset   += bytesToRead;
        }
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Write
    if ( tokens.numParameters() >= 4 && *(tokens.getParameter( 1 )) == 'w' )
    {
        unsigned offset = 0;
        if ( !Cpl::Text::a2ui( offset, tokens.getParameter( 2 ) ) )
        {
            outtext.format( "Invalid 'offset': %s", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        int      bufLen;
        uint8_t* tmpBuf            = (uint8_t*) context.getTokenBuffer().getBuffer( bufLen );
        unsigned numBytes          = tokens.numParameters() - 3;
        int      parmOffset        = 3;
        unsigned totalBytesWritten = 0;
        unsigned startingOffest    = offset;
        while ( numBytes )
        {
            // Get next set of bytes to write
            unsigned bytesToWrite = numBytes > (unsigned) bufLen ? (unsigned) bufLen : numBytes;
            for ( unsigned i=0; i < bytesToWrite; i++ )
            {
                unsigned byte;
                if ( !Cpl::Text::a2ui( byte, tokens.getParameter( parmOffset + i ), 0 ) )
                {
                    outtext.format( "Invalid 'byte' at %d: %s", parmOffset + i, tokens.getParameter( parmOffset + i ) );
                    context.writeFrame( outtext );
                    return Command::eERROR_INVALID_ARGS;
                }
                tmpBuf[i] = byte & 0xFF;
            }

            // Write one buffer at time
            if ( !m_nvDriver.write( offset, tmpBuf, bytesToWrite ) )
            {
                outtext.format( "Write failed.  Offset=%u, len=%u", offset, bytesToWrite );
                io &= context.writeFrame( outtext );
                return Command::eERROR_FAILED;
            }

            parmOffset        += bytesToWrite;
            offset            += bytesToWrite;
            totalBytesWritten += bytesToWrite;
            numBytes          -= bytesToWrite;

        }

        outtext.format( "wrote %u bytes at offset %u ", totalBytesWritten, startingOffest );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // TESTs
    if ( tokens.numParameters() == 3 && *(tokens.getParameter( 1 )) == 't' )
    {
        // Blank check (assumes 0xFF is the 'erased' value)
        if ( strcmp( tokens.getParameter( 2 ), "blank" ) == 0 )
        {
            unsigned long     startTime = Cpl::System::ElapsedTime::milliseconds();
            Command::Result_T result    = readAll( 0xFF, context, outtext );
            unsigned long     endTime   = Cpl::System::ElapsedTime::milliseconds();

            if ( result == Command::eSUCCESS )
            {
                outtext.format( "Verify Blank completed. Duration = %lu ms", endTime - startTime );
                io &= context.writeFrame( outtext );
                return io ? Command::eSUCCESS : Command::eERROR_IO;
            }
            return result;
        }

        // Write 0x55
        if ( strcmp( tokens.getParameter( 2 ), "55" ) == 0 )
        {
            unsigned long     startTime = Cpl::System::ElapsedTime::milliseconds();
            Command::Result_T result    = writeAll( 0x55, context, outtext );
            if ( result == Command::eSUCCESS )
            {
                result = readAll( 0x55, context, outtext );
                unsigned long     endTime   = Cpl::System::ElapsedTime::milliseconds();
                if ( result == Command::eSUCCESS )
                {
                    outtext.format( "Test '55' completed. Duration = %lu ms", endTime - startTime );
                    io &= context.writeFrame( outtext );
                    return io ? Command::eSUCCESS : Command::eERROR_IO;
                }
            }
            return result;
        }

        // Write 0xAA
        if ( strcmp( tokens.getParameter( 2 ), "AA" ) == 0 || strcmp( tokens.getParameter( 2 ), "aa" ) == 0 )
        {
            unsigned long     startTime = Cpl::System::ElapsedTime::milliseconds();
            Command::Result_T result    = writeAll( 0xAA, context, outtext );
            if ( result == Command::eSUCCESS )
            {
                result = readAll( 0xAA, context, outtext );
                unsigned long     endTime   = Cpl::System::ElapsedTime::milliseconds();
                if ( result == Command::eSUCCESS )
                {
                    outtext.format( "Test 'AA' completed. Duration = %lu ms", endTime - startTime );
                    io &= context.writeFrame( outtext );
                    return io ? Command::eSUCCESS : Command::eERROR_IO;
                }
            }
            return result;
        }
    }

    // If I get here -->the argument(s) where bad
    return Cpl::TShell::Command::eERROR_INVALID_ARGS;
}

Cpl::TShell::Command::Result_T Cmd::writeAll( uint8_t value, Cpl::TShell::Context_& context, Cpl::Text::String& outtext )
{
    // Get a work buffer
    int   bufLen;
    char* tmpBuf = context.getTokenBuffer().getBuffer( bufLen );
    memset( tmpBuf, value, bufLen );

    // Write the entire storage
    unsigned      bytesRemaining = m_nvDriver.getTotalSize();
    unsigned      offset         = 0;
    while ( bytesRemaining )
    {
        size_t bytesToWrite = bytesRemaining > (unsigned) bufLen ? (unsigned) bufLen : bytesRemaining;
        if ( !m_nvDriver.write( offset, tmpBuf, bytesToWrite ) )
        {
            outtext.format( "Write failed at offset: %lu", offset );
            context.writeFrame( outtext );
            return Command::eERROR_FAILED;
        }
        bytesRemaining -= bytesToWrite;
        offset         += bytesToWrite;
    }

    return Command::eSUCCESS;
}

Cpl::TShell::Command::Result_T Cmd::readAll( uint8_t expectedVal, Cpl::TShell::Context_& context, Cpl::Text::String& outtext )
{
    // Get a work buffer
    int   bufLen;
    char* tmpBuf = context.getTokenBuffer().getBuffer( bufLen );
    int   buf2Len;
    char* tmpBuf2 = context.getTokenBuffer2().getBuffer( buf2Len );
    if ( bufLen < buf2Len )
    {
        bufLen = buf2Len;
    }
    memset( tmpBuf2, expectedVal, buf2Len );

    // Read the entire storage
    unsigned bytesRemaining = m_nvDriver.getTotalSize();
    unsigned offset         = 0;
    while ( bytesRemaining )
    {
        size_t bytesToRead = bytesRemaining > (unsigned) bufLen ? (unsigned) bufLen : bytesRemaining;
        if ( !m_nvDriver.read( offset, tmpBuf, bytesToRead ) )
        {
            outtext.format( "Read failed at offset: %lu", offset );
            context.writeFrame( outtext );
            return Command::eERROR_FAILED;
        }
        if ( memcmp( tmpBuf, tmpBuf2, bytesToRead ) != 0 )
        {
            outtext.format( "Failed verify check.  Non '%02X' value at offset: %u, len=%lu", expectedVal, offset, bytesToRead );
            context.writeFrame( outtext );
            return Command::eERROR_FAILED;
        }
        bytesRemaining -= bytesToRead;
        offset         += bytesToRead;
    }

    return Command::eSUCCESS;
}