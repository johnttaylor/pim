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


#include "BlockEncoder.h"


///
using namespace Cpl::Text::Frame;



///////////////////////////////////
BlockEncoder::BlockEncoder( void* blockBuffer, size_t blockSizeInBytes, Cpl::Io::Output* dstPtr, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline )
    :StreamEncoder( dstPtr, startOfFrame, endOfFrame, escapeChar, appendNewline )
    , m_buffer( (uint8_t*) blockBuffer )
    , m_bufferSize( blockSizeInBytes )
    , m_bufferCount( 0 )
{
}


///////////////////////////////////
bool BlockEncoder::endFrame() noexcept
{
    // Do the end-of-frame processing
    bool result = StreamEncoder::endFrame();

    // Flush the block buffer to the stream
    if ( result )
    {
        // Note: if m_dstPtr is null, then the endFrame() would have failed -->so I don't need to check for null a second time
        return m_dstPtr->write( m_buffer, m_bufferCount );
    }

    return result;
}

bool BlockEncoder::appendToBlock( char src ) noexcept
{
    // Make sure we HAVE a stream!
    if ( !m_dstPtr )
    {
        return false;
    }

    // Append to the buffer
    if ( m_bufferCount < m_bufferSize )
    {
        m_buffer[m_bufferCount] = (uint8_t) src;
        m_bufferCount++;
    }

    // Flush the buffer to the stream when it is full
    if ( m_bufferCount >= m_bufferSize )
    {
        // Reset my buffer count
        m_bufferCount = 0;

        return m_dstPtr->write( m_buffer, m_bufferSize );
    }

    // If I get here, then I have successful cached in the output data
    return true;
}



bool BlockEncoder::start( char src ) noexcept
{
    m_bufferCount = 0;
    return appendToBlock( src );
}

bool BlockEncoder::start() noexcept
{
    // Nothing to do for stream output
    m_bufferCount = 0;
    return true;
}


bool BlockEncoder::append( char src ) noexcept
{
    return appendToBlock( src );
}
