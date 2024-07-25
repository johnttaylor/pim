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


#include "Driver/Crypto/ED25519.h"
#include "orlp/ed25519/ed25519.h"
#include "Cpl/System/Assert.h"


DriverCryptoStatus_T Driver::Crypto::ED25519::createKeyPair( Keys_T        dstKeys,
                                                             const uint8_t seed[SEED_SIZE] )
{
    ed25519_create_keypair( dstKeys.publicKey, dstKeys.privateKey, seed );
    return DRIVER_CRYPTO_SUCCESS;
}

DriverCryptoStatus_T Driver::Crypto::ED25519::sign( uint8_t      dstSignature[SIGNATURE_SIZE],
                                                    const void*  srcMessage,
                                                    size_t       srcMessageLength,
                                                    const Keys_T keyPair )
{
    ed25519_sign( dstSignature, 
                  (const uint8_t*) srcMessage, 
                  srcMessageLength, 
                  keyPair.publicKey, 
                  keyPair.privateKey );
    return DRIVER_CRYPTO_SUCCESS;
}


DriverCryptoStatus_T Driver::Crypto::ED25519::verify( const uint8_t  messageSignature[SIGNATURE_SIZE],
                                                      const void*    message,
                                                      size_t         messageLength,
                                                      const  uint8_t publicKey[PUBLIC_KEY_SIZE] )
{
    return ed25519_verify( messageSignature, (const uint8_t*) message, messageLength, publicKey );
}
