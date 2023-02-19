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

#include "Input.h"
#include <stdint.h>
#include "pico/stdlib.h"

//
using namespace Cpl::Io::Serial::RP2040::Stdio;



///////////////////
Input::Input()
    : m_nextByte( PICO_ERROR_TIMEOUT )
{
}


//////////////////////
bool Input::read( void* buffer, int numBytes, int& bytesRead )
{
    // Ignore read requests of ZERO bytes
    bytesRead = 0;
    if ( numBytes == 0 )
    {
        return true;
    }

    // Get the cached next byte (when there is one)
    uint8_t* dstPtr = (uint8_t*) buffer;
    if ( m_nextByte != PICO_ERROR_TIMEOUT )
    {
        *dstPtr = (uint8_t) m_nextByte;
        dstPtr++;
        bytesRead++;
        numBytes--;
        m_nextByte = PICO_ERROR_TIMEOUT;
    }

    // Attempt to read the remaining request bytes
    while( numBytes )
    {
        int byte = getchar_timeout_us( 0 );
        
        // No available bytes
        if ( byte == PICO_ERROR_TIMEOUT )
        {
            break;
        }

        *dstPtr = (uint8_t) byte;
        dstPtr++;
        bytesRead++;
        numBytes--;
    }

    // NOTE: The method NEVER returns an error
    return true;
}


bool Input::available()
{
    // If there is cached next byte -->return immediately
    if ( m_nextByte == PICO_ERROR_TIMEOUT )
    {
        return true;
    }

    // Attempt a read...
    m_nextByte = getchar_timeout_us( 0 );
    return m_nextByte != PICO_ERROR_TIMEOUT;
}

bool Input::isEos( void )
{
	return false;   // End-of-Stream has no meaning for the SDK's stdin
}

void Input::close()
{
    // stdin can never be closed - so pretty much ignore the close() call
    m_nextByte = PICO_ERROR_TIMEOUT;
}
