#ifndef Driver_Crypto_Password_Api_h_
#define Driver_Crypto_Password_Api_h_
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



#include "Driver/Crypto/Hash.h"


/// Result: Success
#define DRIVER_CRYPTO_PASSWORD_SUCCESS                  DRIVER_CRYPTO_SUCCESS

/// Result: Output buffer not large enough to hold the hash output
#define DRIVER_CRYPTO_PASSWORD_OUTPUT_BAD_SIZE          (DRIVER_CRYPTO_SUCCESS+1)

/// Result: Provided work buffer not large enough 
#define DRIVER_CRYPTO_PASSWORD_WORK_BUFFER_BAD_SIZE     (DRIVER_CRYPTO_SUCCESS+2)

/// Result: Provided work hash digest buffer not large enough 
#define DRIVER_CRYPTO_PASSWORD_WORK_DIGEST_BAD_SIZE     (DRIVER_CRYPTO_SUCCESS+3)

/// Result: Hash function failure 
#define DRIVER_CRYPTO_PASSWORD_HASH_FUNCTION_ERROR      (DRIVER_CRYPTO_SUCCESS+4)

///
namespace Driver {
///
namespace Crypto {
///
namespace PasswordHash {


/** This method takes plain text plus a 'salt' and generates an hashed 
    output. Expected use is for a primitive 'hash' of password. The
    algorithm is:
    \code

    H = FUNC(HF, plaintext, salt, c)
    where:
        - H is output of FUNC (i.e., the final result of the hashing process).
            o The size, in bytes, of H is 32 bytes (i.e., the size of the HF 
              function’s digest).
        - FUNC is the top-level function.
        - HF is the cryptographic hashing function.
        - salt is a N byte array provided by caller
        - plaintext is ASCII plain text, i.e. the password that is being ‘hashed’.
        - c is the number of iterations to run HF hashing algorithm. The first 
          iteration is performed on plaintext + salt, subsequent iterations use 
          plaintext + <previous-iteration-output> as their input.  The output 
          of each iteration is XOR’d to produce the final result.

            F(plaintext, salt, c) = U1 ^ U2 … ^ Uc
            where:
                U1 = HF(plaintext + salt)
                U2 = HF(plaintext + U1)
                Uc = HF(plaintext + Uc-1)

    \endcode

    Notes: 
        - The size of 'dstOutputBuffer' MUST be greater than or equal to the
          size of the hashFunction's digest size.
        - The caller is required to provide a 'workBuffer' who's size is equal 
          to OR larger than ALL of the values listed below:
            o plaintextLength + saltLength
            o plaintextLength + hashFunction.digestSize()
        - The caller is required to provide additional 'workDigest' memory that
          is used a scratch pad for the hash digest.  The size must be greater
          than or equal to the size of the hashFunction's digest.
 */
DriverCryptoStatus_T hash( const char*           plaintext,
                           size_t                plaintextLength,
                           const void*           salt,
                           size_t                saltLength,
                           uint8_t*              workBuffer,
                           size_t                workBufferLength,
                           uint8_t*              workDigest,
                           size_t                workDigestLength,
                           Driver::Crypto::Hash& hashFunction,
                           size_t                numIterations,
                           void*                 dstOutputBuffer,
                           size_t                dstOutputBufferLen ) noexcept;



} // End namespace(s)
}
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
