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
#include "Storm/Dm/MpVirtualOutputs.h"
#include "Storm/Constants.h"
#include "common.h"
#include <string.h>


using namespace Storm::Dm;

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

static bool compare( Storm::Type::VirtualOutputs_T d, uint16_t odFanOut=0, uint16_t odStage1Out=0, bool sovHeat=false, uint16_t idFanOut=0, uint16_t idFanContOut=0, uint16_t idStage1Out=0 )
{
    return d.outdoorFan == odFanOut && d.outdoorStages[0] == odStage1Out && d.sovInHeating == sovHeat && d.indoorFan == idFanOut && d.indoorFanCont == idFanContOut && d.indoorStages[0] == idStage1Out;
}

////////////////////////////////////////////////////////////////////////////////
// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MpVirtualOutputs          mp_apple_( modelDb_, "APPLE" );
static MpVirtualOutputs          mp_orange_( modelDb_, "ORANGE" );



////////////////////////////////////////////////////////////////////////////////
//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpVirtualOutputs" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    Storm::Type::VirtualOutputs_T value;
    Storm::Type::VirtualOutputs_T expectedVal;
    mp_apple_.setInvalid();

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
        REQUIRE( strcmp( mpType, "Storm::Dm::MpVirtualOutputs" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        MpVirtualOutputs::setSafeAllOff( expectedVal );
        expectedVal.sovInHeating = true;
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.indoorFan = 10;
        mp_apple_.setIndoorFanOutput( expectedVal.indoorFan );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.indoorFanCont = 101;
        mp_apple_.setIndoorFanContinousOutput( expectedVal.indoorFanCont );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 ); 
        
        expectedVal.indoorStages[0] = 102;
        mp_apple_.setIndoorStageOutput( 0, expectedVal.indoorStages[0] );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.outdoorStages[0] = 103;
        mp_apple_.setOutdoorStageOutput( 0, expectedVal.outdoorStages[0] );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.outdoorFan = 104;
        mp_apple_.setOutdoorFanOutput( expectedVal.outdoorFan );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.sovInHeating = false;
        mp_apple_.setSovToCooling();
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.sovInHeating = true;
        mp_apple_.setSovToHeating();
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.outdoorStages[0] = 0;
        expectedVal.outdoorFan       = 0;
        mp_apple_.setOutdoorOff();
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.indoorStages[0] = 0;
        expectedVal.indoorFan       = 0;
        expectedVal.indoorFanCont   = 0;
        mp_apple_.setIndoorOff();
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        MpVirtualOutputs::setSafeAllOff( expectedVal );
        mp_apple_.setSafeAllOff();
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );
    }

    SECTION( "copy" )
    {
        MpVirtualOutputs::setSafeAllOff( expectedVal );
        expectedVal.sovInHeating = true;
        mp_apple_.write( expectedVal );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        mp_orange_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == false );
        mp_apple_.copyFrom( mp_orange_ );
        REQUIRE( mp_apple_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        MpVirtualOutputs::setSafeAllOff( expectedVal );
        expectedVal.indoorFan        = 11;
        expectedVal.indoorFanCont    = 12;
        expectedVal.outdoorFan       = 1;
        expectedVal.sovInHeating     = true;
        expectedVal.outdoorStages[0] = 2;
        expectedVal.indoorStages[0]  = 22;
        mp_apple_.write( expectedVal );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        JsonObject val = doc["val"];
        REQUIRE( val["odFan"] == 1 );
        REQUIRE( val["idFan"] == 11 );
        REQUIRE( val["idFanCont"] == 12 );
        REQUIRE( val["sovHeat"] == true );
        REQUIRE( val["odStages"][0]["stage"] == 1 );
        REQUIRE( val["odStages"][0]["capacity"] == 2 );
        REQUIRE( val["idStages"][0]["stage"] == 1 );
        REQUIRE( val["idStages"][0]["capacity"] == 22 );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{\"name\":\"APPLE\",\"val\":{\"odFan\":1,\"odStages\":[{\"stage\":1,\"capacity\":2}], sovHeat:true, idFan:11, idStages:[{stage:1,capacity:33}],idFanCont:12}}";
        bool result = modelDb_.fromJSON( json );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=(%s)", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( compare( value, 1, 2, true, 11, 12, 33 ) == true );

        json = "{name:\"APPLE\", val:\"abc\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        Cpl::Dm::MailboxServer     t1Mbox;
        Viewer<MpVirtualOutputs>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        MpVirtualOutputs::setSafeAllOff( expectedVal );
        expectedVal.sovInHeating = true;
        mp_apple_.write( expectedVal );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Waiting for viewer signal...") );
        Cpl::System::Thread::wait();
        viewer_apple1.close();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer closed.") );

        // Shutdown threads
        t1Mbox.pleaseStop();
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Destroying Viewer thread (%p)...", t1) );
        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop BEFORE the runnable object goes out of scope
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
