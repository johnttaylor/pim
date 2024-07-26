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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Driver/Crypto/PasswordHash/Api.h"
#include "Driver/Crypto/Orlp/Sha512.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/format.h"

using namespace Driver::Crypto::PasswordHash;

#define HASH_DIGEST_SIZE        64
#define MAX_PLAIN_TEXT_SIZE     64
#define MAX_SALT_SIZE           16
#define MAX_WORK_BUFFER_SIZE    (MAX_PLAIN_TEXT_SIZE+HASH_DIGEST_SIZE) // Hash digest is greater than the salt size


#define GOLDEN_BOB  "0FAD060048CA7374DFC169EED0E9A28A273C52EC0C01A65EA7B171C1296CE17A1314E8F3DA0C1880DB518EB16C356A0D0A241CF41E3D800DBFF70F42D3175FCE"
#define GOLDEN_BOB2 "E8D4FEAD64EFCB59BAB87471475D8F023E01DBBE49C4FA7486622D3DBFF57626CEF4AE9E9BBCDCF37AC5F085F5DEC7850B2EC600891ED95CE8674D0E607ADF61"

#define SECT_       "_0test"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PasshwordHash" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Driver::Crypto::Orlp::SHA512 hf;
    uint8_t workDigest[HASH_DIGEST_SIZE];
    uint8_t workBuffer[MAX_WORK_BUFFER_SIZE];
    uint8_t outputBuffer[HASH_DIGEST_SIZE];
    Cpl::Text::FString<HASH_DIGEST_SIZE * 2> tmpString;

    SECTION( "errors" )
    {
        uint8_t     salt[]     ={ 0x78,0xf4,0x2d,0x65,0xea,0xf6,0x45,0x8c,0xa3,0x05,0xa2,0xbd,0xaf,0x54,0x47,0x4d };
        const char* plaintext  = "myNameIsBob";
        DriverCryptoStatus_T r = hash( plaintext,
                                       strlen( plaintext ),
                                       salt,
                                       sizeof( salt ),
                                       workBuffer,
                                       sizeof( workBuffer ),
                                       workDigest,
                                       sizeof( workDigest ),
                                       hf,
                                       128,
                                       outputBuffer,
                                       1 );
        REQUIRE( r == DRIVER_CRYPTO_PASSWORD_OUTPUT_BAD_SIZE );

        r = hash( plaintext,
                  strlen( plaintext ),
                  salt,
                  sizeof( salt ),
                  workBuffer,
                  1,
                  workDigest,
                  sizeof( workDigest ),
                  hf,
                  128,
                  outputBuffer,
                  sizeof( outputBuffer ) );
        REQUIRE( r == DRIVER_CRYPTO_PASSWORD_WORK_BUFFER_BAD_SIZE );

        r = hash( plaintext,
                  strlen( plaintext ),
                  salt,
                  sizeof( salt ),
                  workBuffer,
                  sizeof( workBuffer ),
                  workDigest,
                  2,
                  hf,
                  128,
                  outputBuffer,
                  sizeof( outputBuffer ) );
        REQUIRE( r == DRIVER_CRYPTO_PASSWORD_WORK_DIGEST_BAD_SIZE );

    }

    SECTION( "golden1" )
    {
        uint8_t     salt[]     ={ 0x78,0xf4,0x2d,0x65,0xea,0xf6,0x45,0x8c,0xa3,0x05,0xa2,0xbd,0xaf,0x54,0x47,0x4d };
        const char* plaintext  = "myNameIsBob";
        DriverCryptoStatus_T r = hash( plaintext,
                                       strlen( plaintext ),
                                       salt,
                                       sizeof( salt ),
                                       workBuffer,
                                       sizeof( workBuffer ),
                                       workDigest,
                                       sizeof( workDigest ),
                                       hf,
                                       128,
                                       outputBuffer,
                                       sizeof( outputBuffer ) );
        Cpl::Text::bufferToAsciiHex( outputBuffer, HASH_DIGEST_SIZE, tmpString );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Hashed value: %s", tmpString.getString()) );
        REQUIRE( r == DRIVER_CRYPTO_SUCCESS );
        REQUIRE( tmpString == GOLDEN_BOB );
    }

    SECTION( "golden2" )
    {
        uint8_t     salt[]     ={ 0x77,0xf4,0x2d,0x65,0xea,0xf6,0x45,0x8c,0xa3,0x05,0xa2,0xbd,0xaf,0x54,0x47,0x4d };
        const char* plaintext  = "myNameIsBob";
        DriverCryptoStatus_T r = hash( plaintext,
                                       strlen( plaintext ),
                                       salt,
                                       sizeof( salt ),
                                       workBuffer,
                                       sizeof( workBuffer ),
                                       workDigest,
                                       sizeof( workDigest ),
                                       hf,
                                       128,
                                       outputBuffer,
                                       sizeof( outputBuffer ) );
        Cpl::Text::bufferToAsciiHex( outputBuffer, HASH_DIGEST_SIZE, tmpString );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Hashed value: %s", tmpString.getString()) );
        REQUIRE( r == DRIVER_CRYPTO_SUCCESS );
        REQUIRE( tmpString == GOLDEN_BOB2 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}