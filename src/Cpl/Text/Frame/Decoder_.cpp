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


#include "Decoder_.h"
#include <memory.h>

///
using namespace Cpl::Text::Frame;




/////////////////////////////////////////////////////////////////////////////
Decoder_::Decoder_( char buffer[], size_t bufsize )
    : m_dataLen( 0 )
    , m_dataPtr( 0 )
    , m_buffer( buffer )
    , m_bufSize( bufsize )
{
    initializeFrame();
}

void Decoder_::initializeFrame() noexcept
{
    m_inFrame   = false;
    m_escaping  = false;
    m_framePtr  = 0;
    m_frameSize = 0;
}


/////////////////////////////////////////////////////////////////////////////
bool Decoder_::scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) noexcept
{
    // Housekeeping
    initializeFrame();

    // Zero out size of the found frame
    frameSize = 0;

    // Error case: Treat a null frame buffer as an IO failure case
    if ( !frame )
    {
        return false;
    }

    // Scan till a frame is found
    for ( ;;)
    {
        // Read N characters at time
        bool isEof;
        if ( !scan( maxSizeOfFrame, frame, frameSize, isEof ) )
        {
            return false;   // Read/IO error occurred
        }
        else if ( isEof )
        {
            return true;    // Frame Found!
        }

    }

    // I should never get here!
    return false;
}


bool Decoder_::scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize, bool& isEof ) noexcept
{
    // Default to in-progress
    isEof = false;

    // Get more input data once my local buffer/cache is empty
    if ( !m_dataLen )
    {
        if ( !read( m_buffer, m_bufSize, m_dataLen ) )
        {
            // Error reading data -->exit scan
            m_dataLen = 0; // Reset my internal count so I start 'over' on the next call (if there is one)
            frameSize = m_frameSize;
            initializeFrame();
            return false;
        }

        // Reset my data pointer
        m_dataPtr = m_buffer;
    }

    // Process my input buffer one character at a time
    for ( ; m_dataLen; m_dataLen--, m_dataPtr++ )
    {
        // OUTSIDE of a frame
        if ( !m_inFrame )
        {
            if ( isStartOfFrame() )
            {
                m_inFrame   = true;
                m_escaping  = false;
                m_frameSize = 0;
                m_framePtr  = frame;
            }
        }

        // INSIDE a frame
        else
        {
            // Trap illegal characters
            if ( !isLegalCharacter() )
            {
                m_inFrame = false;
            }

            // No escape sequence in progress
            else if ( !m_escaping )
            {
                // EOF Character
                if ( isEofOfFrame() )
                {
                    // EXIT routine with a success return code
                    m_dataPtr++;        // Explicitly consume the EOF character (since we are brute force exiting the loop)
                    m_dataLen--;
                    frameSize = m_frameSize;
                    isEof     = true;
                    initializeFrame();  // Reset my internal frame state to be ready for the next frame
                    return true;
                }

                // Regular character
                else if ( !isEscapeChar() )
                {
                    // Store incoming character into the Client's buffer
                    if ( m_frameSize < maxSizeOfFrame )
                    {
                        *m_framePtr++ = *m_dataPtr;
                        m_frameSize++;
                    }

                    // Exceeded the Client's buffer space -->internal error -->reset my Frame state
                    else
                    {
                        initializeFrame();
                    }
                }

                // Start escape sequence
                else
                {
                    m_escaping = true;
                }
            }


            // Escape Sequence
            else
            {
                // Store the escaped character into the Client's buffer
                if ( m_frameSize < maxSizeOfFrame )
                {
                    m_escaping    = false;
                    *m_framePtr++ = decodeEscapedChar( *m_dataPtr );
                    m_frameSize++;
                }

                // Exceeded the Client's buffer space -->internal error -->reset my Frame state
                else
                {
                    initializeFrame();
                }
            }
        }
    }

    // If I get here there was no IO error - but still no End-of-Frame
    frameSize = m_frameSize;
    return true;
}

char Decoder_::decodeEscapedChar( char escapedChar )
{
    return escapedChar;
}

bool Decoder_::oobRead( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    // FAIL if processing a frame
    if ( m_inFrame )
    {
        return false;
    }

    // No cached data 
    if ( !m_dataLen )
    {
        // Read data from the input source into the local cache
        if ( !read( m_buffer, m_bufSize, m_dataLen ) )
        {
            return false;
        }
        m_dataPtr = m_buffer;
    }

    // More data requested than what is cached
    if ( numBytes > m_dataLen )
    {
        bytesRead = m_dataLen;
        m_dataLen = 0;
        memcpy( buffer, m_dataPtr, bytesRead );
        return true;
    }

    // Consume part of the cached data
    memcpy( buffer, m_dataPtr, numBytes );
    m_dataLen -= numBytes;
    m_dataPtr += numBytes;
    bytesRead  = numBytes;
    return true;
}

