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


#include "Driver/Crypto/Api.h"

DriverCryptoStatus_T Driver::Crypto::initialize() noexcept
{
    // Nothing needed
    return DRIVER_CRYPTO_SUCCESS;
}

void Driver::Crypto::shutdown() noexcept
{
    // Nothing needed
}