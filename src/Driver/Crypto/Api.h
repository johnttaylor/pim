#ifndef Driver_Crypto_Api_h_
#define Driver_Crypto_Api_h_
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


#include "colony_config.h"

/// Return Status.  DRIVER_CRYPTO_SUCCESS is success, all other values indicate an error
#ifndef DriverCryptoStatus_T
#define DriverCryptoStatus_T        int
#endif

/// Success return value (Warning: strongly recommended that the value is not overridden)
#ifndef DRIVER_CRYPTO_SUCCESS
#define DRIVER_CRYPTO_SUCCESS       0
#endif

///
namespace Driver {
///
namespace Crypto {

/** This method is used to initialize the Crytpo engine.  It must be called
    before any other Crypto methods.

    Returns DRIVER_CRYPTO_SUCCESS when successful
 */
DriverCryptoStatus_T    initialize() noexcept;


/// This method is used to 'cleanly' shutdown the Crypto engine.
void                    shutdown() noexcept;


} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
