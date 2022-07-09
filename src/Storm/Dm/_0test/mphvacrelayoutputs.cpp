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
#include "Storm/Dm/MpHvacRelayOutputs.h"
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
static MpHvacRelayOutputs          mp_apple_( modelDb_, "APPLE" );
static MpHvacRelayOutputs          mp_orange_( modelDb_, "ORANGE" );



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpHvacRelayOutputs" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    Storm::Type::HvacRelayOutputs_T value;
    Storm::Type::HvacRelayOutputs_T expectedVal;

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
        REQUIRE( strcmp( mpType, "Storm::Dm::MpHvacRelayOutputs" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        Storm::Dm::MpHvacRelayOutputs::setSafeAllOff( expectedVal );
        expectedVal.o = false;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.g  = true;
        expectedVal.y1 = true;
        expectedVal.o  = true;
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        Storm::Dm::MpHvacRelayOutputs::setSafeAllOff( expectedVal );
        mp_apple_.setSafeAllOff();
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        expectedVal.bk = 112;
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        expectedVal.bk = 100;
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );
    }

    SECTION( "copy" )
    {
        Storm::Dm::MpHvacRelayOutputs::setSafeAllOff( expectedVal );
        expectedVal.w1 = true;
        expectedVal.y2 = true;
        expectedVal.o  = false;
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
        Storm::Dm::MpHvacRelayOutputs::setSafeAllOff( expectedVal );
        expectedVal.o  = true;
        expectedVal.w2 = true;
        expectedVal.y1 = true;
        mp_apple_.write( expectedVal );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["g"] == false );
        REQUIRE( doc["val"]["bk"] == 0 );
        REQUIRE( doc["val"]["y1"] == true );
        REQUIRE( doc["val"]["y2"] == false );
        REQUIRE( doc["val"]["w1"] == false );
        REQUIRE( doc["val"]["w2"] == true );
        REQUIRE( doc["val"]["w3"] == false );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{g:true,bk:50,y1:true,y2:false,w1:true,w2:false,w3:true,o:\"COOL\"}}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        Storm::Dm::MpHvacRelayOutputs::setSafeAllOff( expectedVal );
        expectedVal.g  = true;
        expectedVal.bk = 50;
        expectedVal.y1 = true;
        expectedVal.w1 = true;
        expectedVal.w3 = true;
        expectedVal.o  = true;
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        json = "{name:\"APPLE\", val:{g:true,bk:50,y1:true,y2:false,w1:true,w2:false,w3:true,o:\"bob\"}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        json = "{name:\"APPLE\", val:{g:true,bk:50,y1:true,y2:false,w1:true,w2:false,w3:true,o:\"HEAT\"}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        expectedVal.o  = false;
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

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
        Viewer<MpHvacRelayOutputs>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        mp_apple_.setInvalid();
        viewer_apple1.open();
        Storm::Dm::MpHvacRelayOutputs::setSafeAllOff( expectedVal );
        expectedVal.g  = true;
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
