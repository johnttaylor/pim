/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "misc.h"
#include <stdio.h>

//
using namespace Cpl::Text;



bool Cpl::Text::unhex( const char* inString, size_t numCharToScan, uint8_t* outData )
{
    size_t inIdx;
    size_t outIdx;

    for ( inIdx = 0, outIdx=0; inIdx < numCharToScan; outIdx++, inIdx += 2 )
    {
        uint8_t hi = unhexChar( inString[inIdx] );
        uint8_t lo = unhexChar( inString[inIdx + 1] );
        if ( 0xff == hi || 0xff == lo )
        {
            return false;
        }
        outData[outIdx] = (hi << 4) | (lo & 0xf);
    }

    return true;
}

uint8_t Cpl::Text::unhexChar( char c )
{
    // Characters: 0-9
    if ( 0x30 <= c && c <= 0x39 )
    {
        return c - 0x30;
    }

    // Characters: A-F
    else if ( 0x41 <= c && c <= 0x46 )
    {
        return c - 0x41 + 0xa;
    }

    // Characters: a-f
    else if ( 0x61 <= c && c <= 0x69 )
    {
        return c - 0x61 + 0xa;
    }

    // INVALID character
    else
    {
        return 0xff;
    }
}

