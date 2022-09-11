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
/** @file */

#include "Pipe.h"
#include "Cpl/Text/strip.h"
#include <new>

using namespace Driver::TPipe;


//////////////////////////////////////////////////
Pipe::Pipe( Cpl::Container::Map<RxFrameHandlerApi>&   rxFrameHdlrs,
            Cpl::Text::Frame::StreamDecoder&          deframer,
            Cpl::Text::Frame::StreamEncoder&          framer,
            size_t                                    rxBufferSize,
            const char*                               verbDelimiters
)
    : m_rxHandlers( rxFrameHdlrs )
    , m_deframer( deframer )
    , m_framer( framer )
    , m_frameBuffer( nullptr )
    , m_outfdPtr( nullptr )
    , m_frameBufSize( rxBufferSize )
    , m_verbDelimiters( verbDelimiters )
    , m_unknownFrames( 0 )
{
}

Pipe::~Pipe()
{
    stop();
}

//////////////////////////////////////////////////
void Pipe::start( Cpl::Io::Input& inStream, Cpl::Io::Output& outStream ) noexcept
{
    m_frameBuffer   = new (std::nothrow) char[m_frameBufSize + 1]; // add space for the null terminator
    m_unknownFrames = 0;
    m_outfdPtr      = &outStream;
    m_framer.setOutput( outStream );
    m_deframer.setInput( inStream );
}

void Pipe::stop() noexcept
{
    if ( m_frameBuffer != nullptr )
    {
        delete[] m_frameBuffer;
        m_frameBuffer = nullptr;
    }
}

size_t Pipe::getUnknownFrameCount() noexcept
{
    Cpl::System::Mutex::ScopeBlock lock( m_lock );
    return m_unknownFrames;
}

//////////////////////////////////////////////////
bool Pipe::sendCommand( const char* completeCommandText, size_t numBytes ) noexcept
{
    bool success = false;
    Cpl::System::Mutex::ScopeBlock lock( m_lock );

    // Fail the send if start() has not been called
    if ( m_frameBuffer != nullptr )
    {
        success = true;
        success &= m_framer.startFrame();
        success &= m_framer.output( completeCommandText, numBytes );
        success &= m_framer.endFrame();
    }
    
    return success;
}

bool Pipe::sendRawCommand( const char* completeCommandText, size_t numBytes ) noexcept
{
    bool success = false;
    Cpl::System::Mutex::ScopeBlock lock( m_lock );

    // Fail the send if start() has not been called
    if ( m_frameBuffer != nullptr )
    {
        success = m_outfdPtr->write( completeCommandText, numBytes );
    }

    return success;
}

bool Pipe::poll() noexcept
{
    // Skip processing if the Pipe has not been started
    if ( m_frameBuffer != nullptr )
    {
        // Decode the incoming data
        size_t sizeOfFrameFound;
        bool   isEof;
        if ( !m_deframer.scan( m_frameBufSize, m_frameBuffer, sizeOfFrameFound, isEof ) )
        {
            // Error reading raw input
            return false;
        }

        // No frame found -->nothing more to do
        if ( !isEof )
        {
            return true;
        }

        // Null terminate the frame.  By definition sizeOfFrameFound <= m_frameBufSize 
        // AND the buffer size is m_frameBufSize + 1
        m_frameBuffer[sizeOfFrameFound] = '\0'; 
        
        // Extract the command verb
        const char* verbToken    = m_frameBuffer;
        const char* endVerbToken = Cpl::Text::stripNotChars( verbToken, m_verbDelimiters );
        Cpl::Container::KeyStringBuffer verb( verbToken, endVerbToken - verbToken );

        // Look up the frame handler
        RxFrameHandlerApi* frameHandlerPtr;
        if ( (frameHandlerPtr=m_rxHandlers.find( verb )) == 0 )
        {
            // Do nothing if no handler was found (well okay we count the number times this happens)
            Cpl::System::Mutex::ScopeBlock lock( m_lock );
            m_unknownFrames++;
            return true;
        }

        // Execute the frame handler
        frameHandlerPtr->execute( m_frameBuffer );
    }

    return true;
}
