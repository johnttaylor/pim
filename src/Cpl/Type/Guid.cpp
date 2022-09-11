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

#include "Guid.h"
#include "Cpl/Text/misc.h"

using namespace Cpl::Type;

bool Guid_T::operator ==( const Guid_T other ) const
{
    return memcmp( block, other.block, sizeof( block ) ) == 0;
}

bool Guid_T::toString( Cpl::Text::String& formattedOutput, bool withBraces )
{
    formattedOutput.format( "%s%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%s",
                            withBraces ? "{" : "",
                            block[0],
                            block[1],
                            block[2],
                            block[3],
                            block[4],
                            block[5],
                            block[6],
                            block[7],
                            block[8],
                            block[9],
                            block[10],
                            block[11],
                            block[12],
                            block[13],
                            block[14],
                            block[15],
                            withBraces ? "}" : "" );

    return formattedOutput.truncated() == false;
}

bool Guid_T::fromString( const char* stringGuid )
{
    // Check for bad input
    if ( stringGuid == 0 )
    {
        return false;
    }


    // Check for leading/trailing '{}'
    if ( stringGuid[0] == '{' )
    {
        if ( stringGuid[CPL_TYPE_GUID_MAX_FORMATTED_WITH_BRACES_LENGTH - 1] != '}' )
        {
            return false;
        }
        stringGuid++;
    }

    // First block of 8 characters
    uint8_t* binaryOutput = block;
    if ( !Cpl::Text::unhex( stringGuid, 8, binaryOutput ) )
    {
        return false;
    }
    stringGuid   += 8;
    binaryOutput += 4;

    // The 3 blocks of 4 characters
    for ( int i=0; i < 3; i++ )
    {
        // Check for '-'
        if ( *stringGuid != '-' )
        {
            return false;
        }
        stringGuid++;

        // Check for block of 4
        if ( !Cpl::Text::unhex( stringGuid, 4, binaryOutput ) )
        {
            return false;
        }

        // Advance to the next block
        stringGuid   += 4;
        binaryOutput += 2;
    }

    // The last Block of 12
    if ( *stringGuid != '-' )
    {
        return false;
    }
    stringGuid++;
    if ( !Cpl::Text::unhex( stringGuid, 12, binaryOutput ) )
    {
        return false;
    }

    return true;
}
