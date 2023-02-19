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

#include "Output.h"
#include <stdint.h>
#include "pico/stdlib.h"

//
using namespace Cpl::Io::Serial::RP2040::Stdio;


//////////////////////
Output::Output()
{
}


//////////////////////
bool Output::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    // Ignore write requests of ZERO bytes
    bytesWritten = 0;
    if ( maxBytes == 0 )
    {
        return true;
    }


    // perform the write
    const uint8_t* srcPtr = (const uint8_t*) buffer;
    while ( maxBytes )
    {
        putchar_raw( *srcPtr );
        srcPtr++;
        maxBytes--;
        bytesWritten++;
    }
    
    // Flush output since the putchar() semantics do NOT flush the output (stdout wants for a newline to flush the output)
    stdio_flush();

    // NOTE: The method NEVER returns an error
    return true;
}


void Output::flush()
{
    stdio_flush();
}

bool Output::isEos( void )
{
    return false;   // End-of-Stream has no meaning for the SDK's stdout
}

void Output::close()
{
    // Ignore - stdout can never be closed
}
