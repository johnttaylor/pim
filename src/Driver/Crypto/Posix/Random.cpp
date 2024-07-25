/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Driver/Crypto/Random.h"
#include <stdio.h>

DriverCryptoStatus_T Driver::Crypto::generateRandom( void* dstBuffer, size_t numBytesToGenerate ) noexcept
{
    FILE *f = fopen( "/dev/random", "rb" );

    if ( f == NULL ) 
    {
        return 1;
    }

    size_t written = fread( dstBuffer, 1, numBytesToGenerate, f );
    fclose( f );
    return written == numBytesToGenerate? DRIVER_CRYPTO_SUCCESS: 1;
}
