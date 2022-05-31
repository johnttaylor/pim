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
#include "Storm/Dm/MpComfortConfig.h"
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
static MpComfortConfig          mp_apple_( modelDb_, "APPLE" );
static MpComfortConfig          mp_orange_( modelDb_, "ORANGE" );


#define SET_VALUES(v,c,on,off)    v.cph = c; v.minOnTime = on; v.minOffTime = off

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpComfortConfig" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    Storm::Type::ComfortConfig_T value;
    Storm::Type::ComfortConfig_T expectedVal;
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
        REQUIRE( strcmp( mpType, "Storm::Dm::MpComfortConfig" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e6CPH, 10, 20 );
        SET_VALUES( expectedVal.compressorHeating, Storm::Type::Cph::e5CPH, 50, 60 );
        SET_VALUES( expectedVal.indoorHeating, Storm::Type::Cph::e4CPH, 70, 80 );
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e6CPH, 11, 22 );
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e6CPH, 12, 23 );
        mp_apple_.writeCompressorCooling( expectedVal.compressorCooling );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.compressorHeating, Storm::Type::Cph::e6CPH, 13, 24 );
        mp_apple_.writeCompressorHeating( expectedVal.compressorHeating );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        SET_VALUES( expectedVal.indoorHeating, Storm::Type::Cph::e6CPH, 14, 25 );
        mp_apple_.writeIndoorHeating( expectedVal.indoorHeating );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );
    }

    SECTION( "copy" )
    {
        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e6CPH, 10, 20 );
        SET_VALUES( expectedVal.compressorHeating, Storm::Type::Cph::e5CPH, 50, 60 );
        SET_VALUES( expectedVal.indoorHeating, Storm::Type::Cph::e4CPH, 70, 80 );
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
        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e6CPH, 10, 20 );
        SET_VALUES( expectedVal.compressorHeating, Storm::Type::Cph::e5CPH, 50, 60 );
        SET_VALUES( expectedVal.indoorHeating, Storm::Type::Cph::e4CPH, 70, 80 );
        mp_apple_.write( expectedVal );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"]["cmpCool"]["cph"], "e6CPH" ) );
        REQUIRE( doc["val"]["cmpCool"]["minOn"] == 10 );
        REQUIRE( doc["val"]["cmpCool"]["minOff"] == 20 );
        REQUIRE( STRCMP( doc["val"]["cmpHeat"]["cph"], "e5CPH" ) );
        REQUIRE( doc["val"]["cmpHeat"]["minOn"] == 50 );
        REQUIRE( doc["val"]["cmpHeat"]["minOff"] == 60 );
        REQUIRE( STRCMP( doc["val"]["indoorHeat"]["cph"], "e4CPH" ) );
        REQUIRE( doc["val"]["indoorHeat"]["minOn"] == 70 );
        REQUIRE( doc["val"]["indoorHeat"]["minOff"] == 80 );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{cmpCool:{cph:\"e4CPH\",minOn:70,minOff:80},cmpHeat:{cph:\"e6CPH\",minOn:10,minOff:20},indoorHeat:{cph:\"e5CPH\",minOn:50,minOff:60} }}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString() ) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e4CPH, 70, 80 );
        SET_VALUES( expectedVal.compressorHeating, Storm::Type::Cph::e6CPH, 10, 20 );
        SET_VALUES( expectedVal.indoorHeating, Storm::Type::Cph::e5CPH, 50, 60 );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        json = "{name:\"APPLE\", val:{cmpCool:{cph:\"4CPH\",minOn:70,minOff:80}}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

        json = "{name:\"APPLE\", val:{cmpHeat:{cph:\"4CPH\",minOn:70,minOff:80}}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

        json = "{name:\"APPLE\", val:{indoorHeat:{cph:\"4CPH\",minOn:70,minOff:80}}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        Cpl::Dm::MailboxServer     t1Mbox;
        Viewer<MpComfortConfig>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        SET_VALUES( expectedVal.compressorCooling, Storm::Type::Cph::e6CPH, 10, 20 );
        SET_VALUES( expectedVal.compressorHeating, Storm::Type::Cph::e5CPH, 50, 60 );
        SET_VALUES( expectedVal.indoorHeating, Storm::Type::Cph::e4CPH, 70, 80 );
        mp_apple_.write( expectedVal );
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
