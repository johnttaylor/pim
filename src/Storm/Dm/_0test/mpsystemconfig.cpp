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
#include "Storm/Dm/MpSystemConfig.h"
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
static MpSystemConfig          mp_apple_( modelDb_, "APPLE" );
static MpSystemConfig          mp_orange_( modelDb_, "ORANGE" );


#define EQUALS(a,b)             if ( (a) != (b) ) return false
#define FLOAT_EQUALS(a,b)       if ( !Cpl::Math::areFloatsEqual(a,b) ) return false

bool isEqual( const Storm::Type::SystemConfig_T& op1, const Storm::Type::SystemConfig_T& op2 )
{
    EQUALS( op1.numCompressorStages, op2.numCompressorStages );
    EQUALS( op1.numIndoorStages, op2.numIndoorStages );
    EQUALS( op1.totalStages, op2.totalStages );
    FLOAT_EQUALS( op1.gain, op2.gain );
    FLOAT_EQUALS( op1.reset, op2.reset );
    EQUALS( op1.indoorUnitType, op2.indoorUnitType );
    EQUALS( op1.outdoorUnitType, op2.outdoorUnitType );
    EQUALS( op1.currentOpMode, op2.currentOpMode );
    for ( uint16_t i=0; i < OPTION_STORM_MAX_TOTAL_STAGES; i++ )
    {
        EQUALS( op1.stages[i].cph, op2.stages[i].cph );
        EQUALS( op1.stages[i].minOffTime, op2.stages[i].minOffTime );
        EQUALS( op1.stages[i].minOnTime, op2.stages[i].minOnTime );
        FLOAT_EQUALS( op1.stages[i].lowerBound, op2.stages[i].lowerBound );
        FLOAT_EQUALS( op1.stages[i].upperBound, op2.stages[i].upperBound );
        EQUALS( op1.maxPvOut, op2.maxPvOut );
        EQUALS( op1.stages[i].minIndoorFan, op2.stages[i].minIndoorFan );
        EQUALS( op1.stages[i].maxIndoorFan, op2.stages[i].maxIndoorFan );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpSystemConfig" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    Storm::Type::SystemConfig_T value;
    Storm::Type::SystemConfig_T expectedValue;

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
        REQUIRE( strcmp( mpType, "Storm::Dm::MpSystemConfig" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        MpSystemConfig::setToOff( expectedValue );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( isEqual( value, expectedValue ) );

        expectedValue.fanContinuousSpeed = 10;
        expectedValue.stages[1].minOffTime = 22;
        mp_apple_.write( expectedValue );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( isEqual( value, expectedValue ) );
    }

    SECTION( "copy" )
    {
        MpSystemConfig::setToOff( expectedValue );
        expectedValue.fanContinuousSpeed = 100;
        expectedValue.stages[1].minOffTime = 220;
        mp_apple_.write( expectedValue );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( isEqual( value, expectedValue ) );

        mp_orange_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == false );
        mp_apple_.copyFrom( mp_orange_ );
        REQUIRE( mp_apple_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        expectedValue ={ {{1.5F, 10.25F, 2, 3, 4, 5, Storm::Type::Cph::e4CPH}, }, Storm::Type::OperatingMode::eCOOLING, Storm::Type::OduType::eAC, Storm::Type::IduType::eFURNACE, 0, 1, 2, 4, 5.5, 6.5, 7.5 };
        mp_apple_.write( expectedValue );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        JsonObject val = doc["val"];
        REQUIRE( STRCMP( val["opMode"], "eCOOLING" ) );
        REQUIRE( STRCMP( val["iduType"], "eFURNACE" ) );
        REQUIRE( STRCMP( val["oduType"], "eAC" ) );
        REQUIRE( val["numCompStages"] == 0 );
        REQUIRE( val["numIdStages"] == 1 );
        REQUIRE( val["totalStages"] == 2 );
        REQUIRE( val["fanCont"] == 4 );
        REQUIRE( Cpl::Math::areDoublesEqual( val["gain"], 5.5 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( val["reset"], 6.5 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( val["maxPv"], 7.5 ) );
        REQUIRE( val["stages"][0]["stage"] == 1 );
        REQUIRE( Cpl::Math::areFloatsEqual( (float) (val["stages"][0]["lower"].as<double>()), 1.5F ) );
        REQUIRE( Cpl::Math::areFloatsEqual( (float) (val["stages"][0]["upper"].as<double>()), 10.25F ) );
        REQUIRE( val["stages"][0]["minBlower"] == 2 );
        REQUIRE( val["stages"][0]["maxBlower"] == 3 );
        REQUIRE( val["stages"][0]["minOn"] == 4 );
        REQUIRE( val["stages"][0]["minOff"] == 5 );
        REQUIRE( STRCMP( val["stages"][0]["cph"], "e4CPH" ) );
        REQUIRE( val["stages"][1]["stage"] == 2 );
        REQUIRE( Cpl::Math::areFloatsEqual( (float) (val["stages"][1]["lower"].as<double>()), 0.0F ) );
        REQUIRE( Cpl::Math::areFloatsEqual( (float) (val["stages"][1]["upper"].as<double>()), 0.0F ) );
        REQUIRE( val["stages"][1]["minBlower"] == 0 );
        REQUIRE( val["stages"][1]["maxBlower"] == 0 );
        REQUIRE( val["stages"][1]["minOn"] == 0 );
        REQUIRE( val["stages"][1]["minOff"] == 0 );
        REQUIRE( STRCMP( val["stages"][1]["cph"], "e2CPH" ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{opMode:\"eHEATING\", iduType:\"eAIR_HANDLER\",oduType:\"eHP\",numCompStages:1,numIdStages:2,totalStages:2,stages:[{stage:1,lower:1,upper:10,minBlower:1,maxBlower:11},{stage:2,lower:2,upper:20,minBlower:3,maxBlower:4,minOn:11,minOff:22,cph:\"e5CPH\"}], fanCont:5, gain:11.5, reset:0.5, maxPv:100.5}}";
        bool result = modelDb_.fromJSON( json );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON [%s]\nerrorMsg=[%s])", json, errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.currentOpMode == Storm::Type::OperatingMode::eHEATING );
        REQUIRE( value.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.outdoorUnitType == Storm::Type::OduType::eHP );
        REQUIRE( value.numCompressorStages == 1 );
        REQUIRE( value.numIndoorStages == 2 );
        REQUIRE( value.totalStages == 2 );
        REQUIRE( value.fanContinuousSpeed == 5 );
        REQUIRE( Cpl::Math::areFloatsEqual( value.gain, 11.5 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value.reset, 0.5 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value.maxPvOut, 100.5 ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].lowerBound, 1.0F ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].upperBound, 10.0F ) );
        REQUIRE( value.stages[0].minIndoorFan == 1 );
        REQUIRE( value.stages[0].maxIndoorFan == 11 );
        REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].lowerBound, 2.0F ) );
        REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].upperBound, 20.0F ) );
        REQUIRE( value.stages[1].minIndoorFan == 3 );
        REQUIRE( value.stages[1].maxIndoorFan == 4 );
        REQUIRE( value.stages[1].minOnTime == 11 );
        REQUIRE( value.stages[1].minOffTime == 22 );
        REQUIRE( value.stages[1].cph == Storm::Type::Cph::e5CPH );

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
        Viewer<MpSystemConfig>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        mp_apple_.setInvalid();
        viewer_apple1.open();
        MpSystemConfig::setToOff( expectedValue );
        expectedValue.fanContinuousSpeed = 1000;
        mp_apple_.write( expectedValue );
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
