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
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Array_.h"
#include "common.h"
#include <string.h>

#define STRCMP(s1,s2)           (strcmp(s1,s2)==0)
#define MAX_STR_LENG            1024
#define SECT_                   "_0test"

#define MY_ARRAY_SIZE           10

#define INITIAL_VALUE           initValuArray_

#define STREAM_BUFFER_SIZE      1024

static int initValuArray_[MY_ARRAY_SIZE] ={ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

using namespace Cpl::Dm;

// Don't let the Runnable object go out of scope before its thread has actually terminated!
static MailboxServer         t1Mbox_;


////////////////////////////////////////////////////////////////////////////////
namespace {

// Define a concrete 'String' child class
class MyUut : public Mp::NumericArray_<int, MY_ARRAY_SIZE, MyUut>
{
public:
    MyUut( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray_<int, MY_ARRAY_SIZE, MyUut>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'MY_ARRAY_SIZE'
    MyUut( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, int initialValueArray[] )
        : Mp::NumericArray_<int, MY_ARRAY_SIZE, MyUut>( myModelBase, symbolicName, initialValueArray )
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


} // end ANONYMOUS namespace

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MyUut       mp_apple_( modelDb_, "APPLE" );
static MyUut       mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );


////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Array" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    int      readValue[MY_ARRAY_SIZE] ={ 0, };
    char     string[MAX_STR_LENG + 1];
    bool     valid;
    bool     truncated;
    uint16_t seqNum;
    uint16_t seqNum2;
    mp_apple_.setInvalid();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == MY_ARRAY_SIZE * sizeof( int ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == MY_ARRAY_SIZE * sizeof( int ) + sizeof( bool ) + 2 * sizeof( size_t ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
        REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::MyUut" ) == 0 );
    }

    SECTION( "read/writes" )
    {
        mp_orange_.read( readValue, MY_ARRAY_SIZE );
        valid = mp_orange_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        REQUIRE( memcmp( readValue, INITIAL_VALUE, sizeof( readValue ) ) == 0 );

        int arrayVal[] ={ 1,2,3,4,5,6,7,8,9,10 };
        seqNum  = mp_apple_.getSequenceNumber();
        seqNum2 = mp_apple_.write( arrayVal, MY_ARRAY_SIZE );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        REQUIRE( memcmp( arrayVal, readValue, sizeof( readValue ) ) == 0 );
        REQUIRE( seqNum + 1 == seqNum2 );

        int array2[] ={ 12,11,10 };
        seqNum = mp_apple_.write( array2, 3, 2 );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( readValue, 3, 2, &seqNum2 );
        REQUIRE( seqNum2 == seqNum );
        REQUIRE( valid );
        REQUIRE( memcmp( array2, readValue, sizeof( array2 ) ) == 0 );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        int expectedVal[] ={ 1,2,12,11,10,6,7,8,9,10 };
        REQUIRE( memcmp( expectedVal, readValue, sizeof( expectedVal ) ) == 0 );

        mp_apple_.write( array2, 3, MY_ARRAY_SIZE - 1 );
        valid = mp_apple_.read( readValue, 3, MY_ARRAY_SIZE - 1 );
        REQUIRE( valid );
        REQUIRE( memcmp( array2, readValue, sizeof( int ) ) == 0 );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        int expectedVal2[] ={ 1,2,12,11,10,6,7,8,9,12 };
        REQUIRE( memcmp( expectedVal2, readValue, sizeof( expectedVal2 ) ) == 0 );
    }

    SECTION( "copy" )
    {
        mp_apple_.copyFrom( mp_orange_ );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        REQUIRE( memcmp( readValue, INITIAL_VALUE, sizeof( readValue ) ) == 0 );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_orange_.write( INITIAL_VALUE, MY_ARRAY_SIZE );
        mp_orange_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: %s", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["start"] == 0 );
        REQUIRE( doc["val"]["elems"][0] == 10 );
        REQUIRE( doc["val"]["elems"][2] == 8 );
        REQUIRE( doc["val"]["elems"][7] == 3 );
        REQUIRE( doc["val"]["elems"][9] == 1 );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{start:1,elems:[111,222,333]}}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        int expectedVal[] ={ 0,111,222,333,0,0,0,0,0,0 };
        REQUIRE( memcmp( expectedVal, readValue, sizeof( expectedVal ) ) == 0 );

        json = "{name:\"APPLE\", val:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "fromJSON - error cases" )
    {
        mp_orange_.write( INITIAL_VALUE, MY_ARRAY_SIZE );
        const char* json = "{name:\"ORANGE\", val:{start:10,elems:[111,222,333]}}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
        valid = mp_orange_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        REQUIRE( memcmp( INITIAL_VALUE, readValue, sizeof( INITIAL_VALUE ) ) == 0 );

        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: %s", errorMsg.getString()) );
        REQUIRE( result == false );

        json = "{name:\"ORANGE\", val:{elems:[\"abc\",222,333]}}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
        valid = mp_orange_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        REQUIRE( memcmp( INITIAL_VALUE, readValue, sizeof( INITIAL_VALUE ) ) == 0 );

        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: %s", errorMsg.getString()) );
        REQUIRE( result == false );

        json = "{name:\"ORANGE\", val:{start:0}}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
        valid = mp_orange_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        REQUIRE( memcmp( INITIAL_VALUE, readValue, sizeof( INITIAL_VALUE ) ) == 0 );

        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: %s", errorMsg.getString()) );
        REQUIRE( result == false );
    }

    SECTION( "other/error cases" )
    {
        MyUut mp_cherry( modelDb_, "CHERRY", 0 );
        int largeDst[MY_ARRAY_SIZE + 1] ={ 0, };
        valid = mp_cherry.read( largeDst, MY_ARRAY_SIZE +1);
        REQUIRE( valid );
        int expectedVal[MY_ARRAY_SIZE] ={ 0, };
        REQUIRE( memcmp( expectedVal, largeDst, sizeof( expectedVal ) ) == 0 );

        int tooLarge[] ={ 100,2,3,4,5,6,7,8,9,99,11 };
        seqNum  = mp_apple_.getSequenceNumber();
        seqNum2 = mp_apple_.write( tooLarge, MY_ARRAY_SIZE + 1 );
        REQUIRE( seqNum + 1 == seqNum2 );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid );
        int expectedVal2[] ={ 100,2,3,4,5,6,7,8,9,99 };
        REQUIRE( memcmp( expectedVal2, readValue, sizeof( expectedVal2 ) ) == 0 );
    }

    SECTION( "observer" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        Viewer<MyUut>        viewer_apple1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( INITIAL_VALUE, MY_ARRAY_SIZE );
        Cpl::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        Cpl::System::Thread::destroy( *t1 );
    }

    SECTION( "export" )
    {
        //  Export/Import Buffer
        uint8_t streamBuffer[STREAM_BUFFER_SIZE];
        REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == true );
        seqNum  = mp_apple_.getSequenceNumber();
        seqNum2 = mp_apple_.getSequenceNumber();
        size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // Update the MP
        seqNum = mp_apple_.write( INITIAL_VALUE, MY_ARRAY_SIZE );
        REQUIRE( seqNum == seqNum2 + 1 );
        bool    valid;
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( memcmp( INITIAL_VALUE, readValue, sizeof(readValue) ) == 0 );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( valid == false );

        // Update the MP
        int expectedVal[] ={ 4,5,6,7,8,10,11,12,13,14 };
        seqNum = mp_apple_.write( expectedVal, MY_ARRAY_SIZE );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( memcmp( expectedVal, readValue, sizeof( expectedVal ) ) == 0 );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == false );
        b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // invalidate the MP
        seqNum = mp_apple_.setInvalid();
        REQUIRE( seqNum == seqNum2 + 1 );
        REQUIRE( mp_apple_.isNotValid() == true );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( readValue, MY_ARRAY_SIZE );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( memcmp( expectedVal, readValue, sizeof( expectedVal ) ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
