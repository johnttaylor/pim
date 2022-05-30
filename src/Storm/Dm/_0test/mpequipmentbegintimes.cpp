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
#include "Storm/Dm/MpEquipmentBeginTimes.h"
#include "Storm/Constants.h"
#include "common.h"
#include <string.h>


using namespace Storm::Dm;

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define SET_VALUES(v, sec, msec)    v = {sec,msec}

////////////////////////////////////////////////////////////////////////////////
// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MpEquipmentBeginTimes          mp_apple_( modelDb_, "APPLE" );
static MpEquipmentBeginTimes          mp_orange_( modelDb_, "ORANGE" );



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpEquipmentBeginTimes" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    Storm::Type::EquipmentTimes_T value;
    Storm::Type::EquipmentTimes_T expectedVal;
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
        REQUIRE( strcmp( mpType, "Storm::Dm::MpEquipmentBeginTimes" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        Cpl::System::ElapsedTime::Precision_T   outdoorUnitBeginOnTime;     //!< The starting time of when the Outdoor unit (aka the compressor) was turned on
        Cpl::System::ElapsedTime::Precision_T   outdoorUnitBeginOffTime;    //!< The starting time of when the Outdoor unit (aka the compressor) was turned off
        Cpl::System::ElapsedTime::Precision_T   indoorUnitBeginOnTime;      //!< The starting time of when the Indoor unit (aka indoor heat) was turned on
        Cpl::System::ElapsedTime::Precision_T   indoorUnitBeginOffTime;     //!< The starting time of when the Indoor unit (aka indoor heat) was turned off
        Cpl::System::ElapsedTime::Precision_T   systemBeginOnTime;          //!< The starting time of when at least one 'stage' of the SYSTEM was turned on
        Cpl::System::ElapsedTime::Precision_T   systemBeginOffTime;         //!< The starting time of when ALL 'stages' of the SYSTEM was turned off


        SET_VALUES( expectedVal.outdoorUnitBeginOnTime, 1, 0 );
        SET_VALUES( expectedVal.outdoorUnitBeginOffTime, 2, 3 );
        SET_VALUES( expectedVal.indoorUnitBeginOnTime, 22, 33 );
        SET_VALUES( expectedVal.indoorUnitBeginOffTime, 24, 34 );
        SET_VALUES( expectedVal.systemBeginOnTime, 25, 35 );
        SET_VALUES( expectedVal.systemBeginOffTime, 26, 36 );
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.outdoorUnitBeginOnTime, 101, 100 );
        mp_apple_.setOutdoorUnitBeginOnTime( expectedVal.outdoorUnitBeginOnTime );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.outdoorUnitBeginOffTime, 201, 200 );
        mp_apple_.setOutdoorUnitBeginOffTime( expectedVal.outdoorUnitBeginOffTime );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.indoorUnitBeginOnTime, 301, 300 );
        mp_apple_.setIndoorUnitBeginOnTime( expectedVal.indoorUnitBeginOnTime );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.indoorUnitBeginOffTime, 401, 400 );
        mp_apple_.setIndoorUnitBeginOffTime( expectedVal.indoorUnitBeginOffTime );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.systemBeginOnTime, 501, 500 );
        mp_apple_.setSystemBeginOnTime( expectedVal.systemBeginOnTime );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.systemBeginOffTime, 601, 600 );
        mp_apple_.setSystemBeginOffTime( expectedVal.systemBeginOffTime );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );
    }

    SECTION( "copy" )
    {
        SET_VALUES( expectedVal.outdoorUnitBeginOnTime, 1, 0 );
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
        SET_VALUES( expectedVal.outdoorUnitBeginOnTime, 1, 100 );
        SET_VALUES( expectedVal.outdoorUnitBeginOffTime, 2, 200 );
        SET_VALUES( expectedVal.indoorUnitBeginOnTime, 22, 300 );
        SET_VALUES( expectedVal.indoorUnitBeginOffTime, 24, 400 );
        SET_VALUES( expectedVal.systemBeginOnTime, 25, 500 );
        SET_VALUES( expectedVal.systemBeginOffTime, 26, 600 );
        mp_apple_.write( expectedVal );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["beginOutdoorOnTimeSec"].as<double>(), 1.1) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["beginOutdoorOffTimeSec"].as<double>(), 2.2 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["beginIndoorOnTimeSec"].as<double>(), 22.3 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["beginIndoorOffTimeSec"].as<double>(), 24.4 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["beginSystemOnTimeSec"].as<double>(), 25.5 ) );
        REQUIRE( Cpl::Math::areDoublesEqual( doc["val"]["beginSystemOffTimeSec"].as<double>(), 26.6 ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{beginIndoorOnTimeSec:5.5,beginIndoorOffTimeSec:6.6,beginOutdoorOnTimeSec:7.7,beginOutdoorOffTimeSec:8.8,beginSystemOnTimeSec:9.9,beginSystemOffTimeSec:1.02}}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        SET_VALUES( expectedVal.outdoorUnitBeginOnTime, 7, 700 );
        SET_VALUES( expectedVal.outdoorUnitBeginOffTime, 8, 800 );
        SET_VALUES( expectedVal.indoorUnitBeginOnTime, 5, 500 );
        SET_VALUES( expectedVal.indoorUnitBeginOffTime, 6, 600 );
        SET_VALUES( expectedVal.systemBeginOnTime, 9, 900 );
        SET_VALUES( expectedVal.systemBeginOffTime, 1, 20 );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        json = "{name:\"APPLE\", val:{beginIndoorOffTimeSec:abc}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        Cpl::Dm::MailboxServer     t1Mbox;
        Viewer<MpEquipmentBeginTimes>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        SET_VALUES( expectedVal.outdoorUnitBeginOnTime, 1, 100 );
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
