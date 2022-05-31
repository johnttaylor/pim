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

#include "common.h"
#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Enum_.h"
#include "Cpl/Type/enum.h"
#include "Cpl/Itc/CloseSync.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"


#define INITIAL_VALUE       MyEnum::eBLUE

using namespace Cpl::Dm;

////////////////////////////////////////////////////////////////////////////////
BETTER_ENUM( MyEnum, int, eRED, eBLUE, eGREEN );

namespace {
class MyUut : public Mp::Enum_<MyEnum,MyUut>
{
public:
    MyUut( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::Enum_<MyEnum, MyUut>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value
    MyUut( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, MyEnum initialValue )
        : Mp::Enum_<MyEnum, MyUut>( myModelBase, symbolicName, initialValue )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::MyUut";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MyUut> Observer;
};

} // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MyUut       mp_apple_( modelDb_, "APPLE" );
static MyUut       mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );


// Don't let the Runnable object go out of scope before its thread has actually terminated!
static MailboxServer         t1Mbox_;


////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Enum" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    MyEnum value = MyEnum::eRED;
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
        REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::MyUut" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        mp_apple_.write( MyEnum::eRED );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyEnum::eRED );
        mp_apple_.write( MyEnum::eGREEN );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyEnum::eGREEN );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +INITIAL_VALUE );
    }

    SECTION( "copy" )
    {
        mp_apple_.write( MyEnum::eRED );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_apple_.read( value );
        REQUIRE( value == +MyEnum::eRED );
        REQUIRE( valid );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }


    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( MyEnum::eBLUE );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"], (+MyEnum::eBLUE)._to_string() ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:\"eGREEN\"}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyEnum::eGREEN );

        json = "{name:\"APPLE\", val:\"true\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

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
        Viewer<MyUut>        viewer_apple1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( MyEnum::eRED );
        Cpl::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        Cpl::System::Thread::destroy( *t1 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
