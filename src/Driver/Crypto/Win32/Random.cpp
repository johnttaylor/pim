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
#include <windows.h>
#include <wincrypt.h>

DriverCryptoStatus_T Driver::Crypto::generateRandom( void* dstBuffer, size_t numBytesToGenerate ) noexcept
{
    HCRYPTPROV prov;

    if ( !CryptAcquireContext( &prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )  
    {
        return GetLastError();
    }

    if ( !CryptGenRandom( prov, numBytesToGenerate, (BYTE*)dstBuffer ) )  
    {
        DWORD err = GetLastError();
        CryptReleaseContext( prov, 0 );
        return err;
    }

    CryptReleaseContext( prov, 0 );
    return DRIVER_CRYPTO_SUCCESS;
}