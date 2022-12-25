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

#include "common.h"
#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/String.h"
#include "Cpl/Itc/CloseSync.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define MY_UUT_DATA_SIZE    8

#define INITIAL_VALUE       "Hello World"

using namespace Cpl::Dm;


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::String<MY_UUT_DATA_SIZE> mp_apple_( modelDb_, "APPLE" );
static Mp::String<MY_UUT_DATA_SIZE> mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );

// Don't let the Runnable object go out of scope before its thread has actually terminated!
static MailboxServer         t1Mbox_;



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "String" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG>     errorMsg = "noerror";
    Cpl::Text::FString<MY_UUT_DATA_SIZE> valStr;
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    mp_apple_.setInvalid();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == MY_UUT_DATA_SIZE+1 );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == MY_UUT_DATA_SIZE + 1 + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
        REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::String" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        mp_apple_.write( "12345678" );
        valid = mp_apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "12345678"  );
        mp_apple_.write( "bob" );
        valid = mp_apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "bob" );

        valid = mp_orange_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "Hello Wo" );

        mp_apple_.write( "Very Long String" );
        valid = mp_apple_.read( string, sizeof( string ) );
        REQUIRE( valid == true );
        REQUIRE( STRCMP( string, "Very Lon" ) );

        mp_apple_.read( string, 4 );
        REQUIRE( STRCMP( string, "Ver" ) );

        uint16_t seqNum  = mp_apple_.getSequenceNumber();
        uint16_t seqNum2 = mp_apple_.write( 0, 10 );
        REQUIRE( seqNum == seqNum2 );
        mp_apple_.read( valStr );
        REQUIRE( valStr == "Very Lon" );
    }

    SECTION( "copy" )
    {
        mp_apple_.write( "charlie" );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "charlie" );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( "Hi Bob" );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["maxLen"] == MY_UUT_DATA_SIZE );
        REQUIRE( STRCMP(doc["val"]["text"], "Hi Bob") );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{text:\"good bye\"}}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "good bye" );

        json = "{name:\"APPLE\", val:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        Cpl::Text::FString<10> expectedVal = "bob";
        Viewer<Mp::String<MY_UUT_DATA_SIZE>, Cpl::Text::FString<10>>  viewer_apple1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_, expectedVal );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( expectedVal );
        Cpl::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop BEFORE the runnable object goes out of scope
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
