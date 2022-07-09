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
#include "Cpl/Type/Guid.h"    
#include "Cpl/Text/FString.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <stdint.h>
#include "colony_map.h"


/// 
using namespace Cpl::Type;
using namespace Cpl::Text;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "guid" )
{

    Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<CPL_TYPE_GUID_MAX_FORMATTED_WITH_BRACES_LENGTH> stringBuffer;
    Guid_T guid ={ 0x12, 0x3e, 0x45, 0x67, 0xe8, 0x9b, 0x12, 0xd3, 0xa4, 0x56, 0x42, 0x66, 0x14, 0x17, 0x40, 0x00 };
    bool result = guid.toString( stringBuffer );
    REQUIRE( result == true );
    REQUIRE( stringBuffer == "123e4567-e89b-12d3-a456-426614174000" );
    result = guid.toString( stringBuffer, true );
    REQUIRE( result == true );
    REQUIRE( stringBuffer == "{123e4567-e89b-12d3-a456-426614174000}" );

    Cpl::Text::FString<CPL_TYPE_GUID_MAX_FORMATTED_LENGTH - 1> tooSmall;
    result = guid.toString( tooSmall );
    REQUIRE( result == false );
    result = guid.toString( tooSmall, true );
    REQUIRE( result == false );

    Guid_T guidOut;
    result = guidOut.fromString( "123e4567-e89b-12d3-a456-426614174000" );
    REQUIRE( result == true );
    REQUIRE( guid == guidOut );
    result = guidOut.fromString( "{123e4567-e89b-12d3-a456-426614174000}" );
    REQUIRE( result == true );
    REQUIRE( guid == guidOut );

    result = guidOut.fromString( "123e4567e89b-12d3-a456-426614174000}" );
    REQUIRE( result == false );
    result = guidOut.fromString( "{123e4567-e89b-12d3-a456-426614174000" );
    REQUIRE( result == false );
    result = guidOut.fromString( "123e4567e89b-12d3-a456-426614174000}" );
    REQUIRE( result == false );
    result = guidOut.fromString( 0 );
    REQUIRE( result == false );

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
