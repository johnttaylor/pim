#ifndef Driver_Crypto_Random_h_
#define Driver_Crypto_Random_h_
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


#include "Driver/Crypto/Api.h"
#include <stdlib.h>

///
namespace Driver {
///
namespace Crypto {

/** This method is used to generate random bytes.  'dstBuffer' must be 
    at least 'numBytesToGenerate' in size. Returns DRIVER_CRYPTO_SUCCESS when 
    successful.

    Note: How "true" the random generator is platform specific.
 */
DriverCryptoStatus_T generateRandom( void* dstBuffer, size_t numBytesToGenerate) noexcept;


} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
