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
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Double.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define MAGIC_VALUE         3.14
#define MAGIC_INC           0.0000001


////////////////////////////////////////////////////////////////////////////////
using namespace Cpl::Dm;

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Double       mp_apple_( modelDb_, "APPLE" );
static Mp::Double       mp_orange_( modelDb_, "ORANGE", MAGIC_VALUE );

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Double" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    char    string[MAX_STR_LENG + 1];
    bool    truncated;
    bool    valid;
    double  value;

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( value ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( value ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
        REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::Double" ) == 0 );
    }


    SECTION( "read/writes/compare" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        mp_apple_.increment( 2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 2 );  // By design the invalid MP has a 'data value' of zero
        uint16_t seqNum = mp_apple_.write( MAGIC_VALUE );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areDoublesEqual( value, MAGIC_VALUE ) );
        uint16_t seqNum2 = mp_apple_.increment( MAGIC_INC );
        mp_apple_.read( value );
        REQUIRE( Cpl::Math::areDoublesEqual( value, MAGIC_VALUE + MAGIC_INC ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areDoublesEqual( value, MAGIC_VALUE ) );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( 127 );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"] == 127 );
    }

    SECTION( "fromJSON" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        const char* json = "{name:\"APPLE\", val:1234}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1234 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
