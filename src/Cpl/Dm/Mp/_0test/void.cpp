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
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Void.h"
#include "common.h"
#include <string.h>

#define STRCMP(s1,s2)           (strcmp(s1,s2)==0)
#define MAX_STR_LENG            2048
#define SECT_                   "_0test"

#define INITIAL_VALUE           ((void*) 0x123)
#define INITIAL_VALUE_STR       "123"

#define STREAM_BUFFER_SIZE      2048


using namespace Cpl::Dm;



////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Void       mp_apple_( modelDb_, "APPLE" );
static Mp::Void       mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );

// Don't let the Runnable object go out of scope before its thread has actually terminated!
static MailboxServer         t1Mbox_;



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to the concrete MP type
//
TEST_CASE( "Void" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    void*    readValue;
    char     string[MAX_STR_LENG + 1];
    bool     truncated;
    bool     valid;
    mp_apple_.setInvalid();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( readValue ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( readValue ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s]", mpType ) );
        REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::Void" ) == 0 );
    }


    SECTION( "toJSON-pretty" )
    {
        mp_orange_.write( INITIAL_VALUE );
        mp_orange_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: (%d) %s", truncated, string ) );
        REQUIRE( truncated == false );

        StaticJsonDocument<2048> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"], INITIAL_VALUE_STR ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:\"FE21\"}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( readValue );
        REQUIRE( valid );
        REQUIRE( readValue == (void*) 0xFE21 );

        json = "{name:\"APPLE\", val:\"true\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        Viewer<Mp::Void> viewer_apple1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( INITIAL_VALUE );
        Cpl::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        Cpl::System::Thread::destroy( *t1 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
