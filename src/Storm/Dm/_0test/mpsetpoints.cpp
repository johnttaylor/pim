/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Math/real.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Storm/Dm/MpSetpoints.h"
#include "Storm/Constants.h"
#include "common.h"
#include <string.h>


using namespace Storm::Dm;

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

////////////////////////////////////////////////////////////////////////////////
// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MpSetpoints          mp_apple_( modelDb_, "APPLE" );
static MpSetpoints          mp_orange_( modelDb_, "ORANGE" );



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpSetpoints" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    float coolValue;
    float heatValue;
    float expectedCoolValue;
    float expectedHeatValue;

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( coolValue ) + sizeof( heatValue ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( coolValue ) + sizeof( heatValue ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
        REQUIRE( strcmp( mpType, "Storm::Dm::MpSetpoints" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        expectedCoolValue = OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_COOLING;
        expectedHeatValue = OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_HEATING;
        valid = mp_apple_.read( coolValue, heatValue );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue) );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, expectedHeatValue) );

        expectedCoolValue = expectedHeatValue;  // Forces adjustment to the Heat setpoint
        expectedHeatValue = expectedCoolValue - 1.0F;
        mp_apple_.writeCool( expectedCoolValue );
        valid = mp_apple_.read( coolValue, heatValue );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, expectedHeatValue ) );

        coolValue = 0.0F;
        valid = mp_apple_.readCool( coolValue );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );
        
        heatValue = 0.0F;
        valid = mp_apple_.readHeat( heatValue );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, expectedHeatValue ) );

        expectedHeatValue = OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING - 1.0F;
        mp_apple_.writeHeat( expectedHeatValue );
        valid = mp_apple_.read( coolValue, heatValue );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING ) );

        expectedCoolValue = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
        expectedHeatValue = OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING + 1.0F;
        mp_apple_.write( expectedCoolValue, expectedHeatValue );
        valid = mp_apple_.read( coolValue, heatValue );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING ) );

        expectedCoolValue = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING + 1.0F;
        mp_apple_.writeCool( expectedCoolValue );
        valid = mp_apple_.readCool( coolValue);
        REQUIRE( valid );
        expectedCoolValue = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );

        expectedCoolValue = OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING - 1.0F;
        mp_apple_.writeCool( expectedCoolValue );
        valid = mp_apple_.readCool( coolValue );
        REQUIRE( valid );
        expectedCoolValue = OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING;
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );
    }

    SECTION( "copy" )
    {
        expectedCoolValue = 72.0F;
        expectedHeatValue = 70.2F;
        mp_apple_.write( expectedCoolValue, expectedHeatValue );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( coolValue, heatValue );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, expectedCoolValue ) );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, expectedHeatValue ) );

        mp_orange_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == false );
        mp_apple_.copyFrom( mp_orange_ );
        REQUIRE( mp_apple_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.setInvalid();
        expectedCoolValue = 82.0F;
        expectedHeatValue = 71.2F;
        mp_apple_.write( expectedCoolValue, expectedHeatValue );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( Cpl::Math::areFloatsEqual( doc["val"]["cool"], expectedCoolValue ) );
        REQUIRE( Cpl::Math::areFloatsEqual( doc["val"]["heat"], expectedHeatValue ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{cool:75.3,heat:71.0}}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( coolValue, heatValue );
        REQUIRE( valid );
        REQUIRE( Cpl::Math::areFloatsEqual( coolValue, 75.3F ) );
        REQUIRE( Cpl::Math::areFloatsEqual( heatValue, 71.0F ) );

        json = "{name:\"APPLE\", val:{g:abc}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

        json = "{name:\"APPLE\", val:\"abc\"}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        Cpl::Dm::MailboxServer        t1Mbox;
        Viewer<MpSetpoints>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        mp_apple_.setInvalid();
        viewer_apple1.open();
        mp_apple_.writeCool( 78.3F );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Waiting for viewer signal...") );
        Cpl::System::Thread::wait();
        viewer_apple1.close();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer closed.") );

        // Shutdown threads
        t1Mbox.pleaseStop();
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Destroying Viewer thread (%p)...", t1) );
        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop BEFORE the runnable object goes out of scope
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
