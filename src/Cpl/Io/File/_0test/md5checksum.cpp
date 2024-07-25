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

#include "Catch/catch.hpp"
#include "Cpl/Io/File/Md5Checksum.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"


#define SECT_     "_0test"

/// 
using namespace Cpl::Io::File;

#define GOLDEN_HASH               "d195d9df44d856e64d9ea8c5b62f4953"
static uint8_t expectedDigest_[] ={ 0xd1,0x95,0xd9,0xdf,0x44,0xd8,0x56,0xe6,0x4d,0x9e,0xa8,0xc5,0xb6,0x2f,0x49,0x53 };

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "md5checksum" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    size_t filelen = 0;
    Cpl::Text::FString<Cpl::Checksum::ApiMd5::eDIGEST_LEN * 2> dstString;
    Cpl::Checksum::ApiMd5::Digest_T digest;
    bool result = calcMD5Checksum( "testinput.txt", digest, &dstString, &filelen );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("md5=[%s]", dstString.getString()) );

    REQUIRE( result );
    REQUIRE( dstString == GOLDEN_HASH );
    REQUIRE( memcmp( expectedDigest_, digest, sizeof( digest ) ) == 0 );
    REQUIRE( filelen == 101 );

    result = calcMD5Checksum( "testinput.txt", digest );
    REQUIRE( result );
    REQUIRE( memcmp( expectedDigest_, digest, sizeof( digest ) ) == 0 );

    filelen = 0;
    result  = calcMD5Checksum( "testinput.txt", digest, nullptr, &filelen );
    REQUIRE( result );
    REQUIRE( memcmp( expectedDigest_, digest, sizeof( digest ) ) == 0 );
    REQUIRE( filelen == 101 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

