#ifndef Driver_Crypto_ED25519_h_
#define Driver_Crypto_ED25519_h_
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
#include <stdint.h>
#include <stdlib.h>


///
namespace Driver {
///
namespace Crypto {

/** This class implements the Edwards-curve digital signature defined by
    Edwards-Curve Digital Signature Algorithm (EdDSA) [RFC8032], using standard
    parameters.
 */
class ED25519
{
public:
    /// Public key size in bytes
    static constexpr size_t PUBLIC_KEY_SIZE = 32;

    /// Public key size in bytes
    static constexpr size_t PRIVATE_KEY_SIZE = 64;

    /// Signature size in bytes
    static constexpr size_t SIGNATURE_SIZE  = 64;

    /// Seed (for when creating a Key par) size in bytes
    static constexpr size_t SEED_SIZE       = 32;

public:
    /// Asymmetrically Key Pair
    struct Keys_T
    {
        uint8_t  privateKey[PRIVATE_KEY_SIZE];  //!< Public Key
        uint8_t  publicKey[PUBLIC_KEY_SIZE];    //!< Private Key
    };

public:
    /** This method create a key pair for the signing algorithm.  The method
        returns DRIVER_CRYPTO_SUCCESS when successful.
     */
    static DriverCryptoStatus_T createKeyPair( Keys_T        dstKeys,
                                               const uint8_t seed[SEED_SIZE] );

    /** This method is used to digital sign a 'message'. The method
        returns DRIVER_CRYPTO_SUCCESS when successful.
     */
    static DriverCryptoStatus_T sign( uint8_t      dstSignature[SIGNATURE_SIZE],
                                      const void*  srcMessage,
                                      size_t       srcMessageLength,
                                      const Keys_T keyPair );


    /** This method is used to verify that the specified 'messages' has been
        signed. The method returns DRIVER_CRYPTO_SUCCESS when message is
        successfully verified.
     */
    static DriverCryptoStatus_T verify( const uint8_t  messageSignature[SIGNATURE_SIZE],
                                        const void*    message,
                                        size_t         messageLength,
                                        const  uint8_t publicKey[PUBLIC_KEY_SIZE] );
 
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
