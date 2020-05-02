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
#include "Cpl/Dm/Mp/RefCounter.h"
#include "common.h"
#include <string.h>

#define STRCMP(s1,s2)  (strcmp(s1,s2)==0)


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StaticInfo      info_mp_apple_( "APPLE" );
static Mp::RefCounter  mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo      info_mp_orange_( "ORANGE" );
static Mp::RefCounter  mp_orange_( modelDb_, info_mp_orange_, 64 );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "refcounter-readwrite", "[refcounter-readwrite]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-READWRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Read
    uint32_t value;
    uint16_t seqNum;
    int8_t   valid = mp_orange_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 64 );
    valid = mp_apple_.read( value, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Write (reset). -->Count = 0
    uint16_t seqNum2 = mp_apple_.reset();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write (decrement)  -->Count = 0
    seqNum = mp_apple_.decrement();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum == seqNum2 );

    // Write (increment)  -->Count = 1
    seqNum2 = mp_apple_.increment();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 1 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write (decrement) -->Count = 0
    seqNum = mp_apple_.decrement();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum == seqNum2 + 1 );

    // Write (decrement) -->Count = 0
    seqNum2 = mp_apple_.decrement();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 0 );
    REQUIRE( seqNum == seqNum2 );       // No a change  transition -->so no change in the sequence number

    // Write (increment)  -->Count = max value
    seqNum2 = mp_apple_.increment( (uint32_t) -1 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == (uint32_t) -1 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write (increment) -->Count = max value
    seqNum = mp_apple_.increment();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == (uint32_t) -1 );
    REQUIRE( seqNum == seqNum2 );

    // Write with lock (decrement) -->Count = 2
    seqNum2 = mp_apple_.decrement( (uint32_t) -3, ModelPoint::eLOCK );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 2 );
    REQUIRE( seqNum == seqNum2 );   // No a change  transition -->so no change in the sequence number

    // Write (increment) -->Count = 2
    seqNum = mp_apple_.increment();
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 2 );
    REQUIRE( seqNum == seqNum2 );

    // Invalidate with Unlock
    mp_apple_.setInvalidState( 112, ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    valid = mp_apple_.getValidState();
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    REQUIRE( valid == 112 );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "refcounter-get", "[refcounter-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-GET test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Gets...
    const char* name = mp_apple_.getName();
    REQUIRE( strcmp( name, "APPLE" ) == 0 );
    name = mp_orange_.getName();
    REQUIRE( strcmp( name, "ORANGE" ) == 0 );

    size_t s = mp_apple_.getSize();
    REQUIRE( s == sizeof( uint32_t ) );
    s = mp_orange_.getSize();
    REQUIRE( s == sizeof( uint32_t ) );

    s = mp_apple_.getExternalSize();
    REQUIRE( s == sizeof( uint32_t ) + sizeof( int8_t ) );
    s = mp_orange_.getExternalSize();
    REQUIRE( s == sizeof( uint32_t ) + sizeof( int8_t ) );

    const char* mpType = mp_apple_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s])", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::RefCounter" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "refcounter-export", "[refcounter-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-EXPORT test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //  Export/Import Buffer
    uint8_t streamBuffer[STREAM_BUFFER_SIZE];
    REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );


    // Export...
    REQUIRE( mp_apple_.isNotValid() == true );
    uint16_t seqNum  = mp_apple_.getSequenceNumber();
    uint16_t seqNum2 = mp_apple_.getSequenceNumber();
    size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Update the MP
    seqNum = mp_apple_.reset( 42 );
    REQUIRE( seqNum == seqNum2 + 1 );
    uint32_t value;
    int8_t   valid;
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == 42 );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Update the MP
    seqNum = mp_apple_.reset( 13 );
    REQUIRE( seqNum == seqNum2 + 1 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == 13 );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // set a new value AND invalidate the MP
    mp_apple_.reset();
    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum == seqNum2 + 2 );
    REQUIRE( mp_apple_.isNotValid() == true );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 13 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
static Cpl::Dm::MailboxServer     t1Mbox_;

#define NUM_CHANGE_NOTIFICATIONS    5 // Register, Invalid ->1, 1->0, 0->Invalid
#define DELAY_TO_NOT_MISS_EDGES     100

TEST_CASE( "refcounter-observer", "[refcounter-observer]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-OBSERVER test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    ViewerRefCounter viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_, NUM_CHANGE_NOTIFICATIONS );

    // Open, write a value, wait for Viewer to see the change, then close
    mp_apple_.removeLock();
    mp_apple_.setInvalid();
    viewer1.open();

    // Invalid ->1..4
    uint16_t seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 4..1 ->0
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 0 -> 0 (no change notification)
    seqNum = mp_apple_.decrement();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 0 -> 1
    seqNum = mp_apple_.increment();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    // 1 -> Invalid
    seqNum = mp_apple_.setInvalid();
    Cpl::System::Api::sleep( DELAY_TO_NOT_MISS_EDGES );

    Cpl::System::Thread::wait();
    viewer1.close();
    REQUIRE( viewer1.m_lastSeqNumber == seqNum );

    // Shutdown threads
    t1Mbox_.pleaseStop();
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    Cpl::System::Thread::destroy( *t1 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    1024

TEST_CASE( "refcounter-toJSON" )
{
	CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-TOJSON test" );
	Cpl::System::Shutdown_TS::clearAndUseCounter();
	char string[MAX_STR_LENG + 1];
	bool truncated;

    SECTION( "Invalid/terse" )
    {
        // Invalid (Default value)
        mp_apple_.setInvalid();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: terse [%s])", string ) );
        REQUIRE( truncated == false );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( STRCMP( doc["name"], "APPLE" ) );
        REQUIRE( doc["type"].as<char*>() == 0 );
        REQUIRE( doc["seqnum"].as<char*>() == 0 );
        REQUIRE( doc["locked"].as<char*>() == 0 );
        REQUIRE( doc["invalid"] > 0 );
    }

	SECTION( "Invalid" )
	{
		// Invalid (Default value)
		uint16_t seqnum = mp_apple_.setInvalid();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );
		REQUIRE( truncated == false );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( STRCMP( doc["name"], "APPLE" ) );
		REQUIRE( STRCMP( doc["type"], mp_apple_.getTypeAsText() ) );
		REQUIRE( doc["seqnum"] == seqnum );
		REQUIRE( doc["locked"] == false );
		REQUIRE( doc["invalid"] > 0 );
	}

	SECTION( "Invalid + Locked" )
	{
		mp_apple_.applyLock();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["locked"] == true );
		REQUIRE( doc["invalid"] > 0 );
	}

	SECTION( "Invalid - custom value" )
	{
		mp_apple_.removeLock();
		uint16_t seqnum = mp_apple_.setInvalidState( 100 );
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["seqnum"] == seqnum );
		REQUIRE( doc["invalid"] == 100 );
		REQUIRE( doc["locked"] == false );
	}

	SECTION( "Invalid - custom value + locked" )
	{
		// Invalid (custom value) + Locked
		mp_apple_.applyLock();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["invalid"] == 100 );
		REQUIRE( doc["locked"] == true );
	}

	SECTION( "Value" )
	{
		uint16_t seqnum = mp_apple_.reset( 2, ModelPoint::eUNLOCK );
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["seqnum"] == seqnum );
		REQUIRE( doc["locked"] == false );
		REQUIRE( doc["invalid"] == 0 );
		REQUIRE( STRCMP( doc["val"], "2" ) );
	}

	SECTION( "Value + Lock" )
	{
		mp_apple_.applyLock();
		mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

		StaticJsonDocument<1024> doc;
		DeserializationError err = deserializeJson( doc, string );
		REQUIRE( err == DeserializationError::Ok );
		REQUIRE( doc["locked"] == true );
		REQUIRE( doc["invalid"] == 0 );
		REQUIRE( STRCMP( doc["val"], "2" ) );
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "refcounter-fromJSON" )
{
	CPL_SYSTEM_TRACE_SCOPE( SECT_, "REFCOUNTER-FROMJSON test" );
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	// Start with MP in the invalid state
	Cpl::Text::FString<MAX_STR_LENG> string;
	Cpl::Text::DString               errorMsg( "noerror", 1024 );
	mp_apple_.removeLock();
	mp_orange_.removeLock();
	mp_orange_.setInvalid();
	uint16_t seqNum = mp_apple_.setInvalid();
	uint16_t seqNum2;
	ModelPoint* mp;

	SECTION( "Write value" )
	{
		const char* json = "{name:\"APPLE\", val:\"3\"}";
		bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( seqNum2 == seqNum + 1 );
		uint32_t value;
		int8_t   valid = mp_apple_.read( value, &seqNum );
		REQUIRE( seqNum == seqNum2 );
		REQUIRE( ModelPoint::IS_VALID( valid ) );
		REQUIRE( value == 3 );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp == &mp_apple_ );

		json = "{name:\"APPLE\", val:\"+4\"}";
		result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( seqNum2 == seqNum  );	// NOTE: The sequence number does NOT change because that change was a transition around zero
		valid = mp_apple_.read( value, &seqNum );
		REQUIRE( seqNum == seqNum2 );
		REQUIRE( ModelPoint::IS_VALID( valid ) );
		REQUIRE( value == 3+4 );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp == &mp_apple_ );

		json = "{name:\"APPLE\", val:\"-7\"}";
		result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( seqNum2 == seqNum + 1 );
		valid = mp_apple_.read( value, &seqNum );
		REQUIRE( seqNum == seqNum2 );
		REQUIRE( ModelPoint::IS_VALID( valid ) );
		REQUIRE( value == 3 + 4 - 7);
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp == &mp_apple_ );
	}

	SECTION( "Write value - error cases" )
	{
		const char* json   = "{name:\"APPLE\", val:12}";
		bool        result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\"}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{namex:\"APPLE\"}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\", val:true}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"APPLE\", val:\"abc\"}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );

		errorMsg = "noerror";
		json     = "{name:\"BOB\", invalid:1}";
		result   = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == false );
	}



	SECTION( "Set Invalid" )
	{
		seqNum = mp_apple_.reset( 12 );
		const char* json = "{name:\"APPLE\", val:\"4321\", invalid:1}";
		bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( seqNum2 == seqNum + 1 );
		uint32_t  value;
		int8_t valid = mp_apple_.read( value, &seqNum );
		REQUIRE( seqNum == seqNum2 );
		REQUIRE( ModelPoint::IS_VALID( valid ) == false );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp == &mp_apple_ );
	}

	SECTION( "lock..." )
	{
		const char* json = "{name:\"APPLE\", val:\"4321\", locked:true}";
		bool result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		uint32_t    value;
		int8_t   valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( errorMsg == "noerror" );
		REQUIRE( mp_apple_.isLocked() == true );
		REQUIRE( value == 4321 );

		json   = "{name:\"APPLE\", invalid:21, locked:false}";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( mp_apple_.isNotValid() == true );
		REQUIRE( mp_apple_.isLocked() == false );
		REQUIRE( mp_apple_.getValidState() == 21 );

		json   = "{name:\"APPLE\", val:\"21\", locked:true}";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( mp_apple_.isLocked() == true );
		valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( value == 21 );

		json   = "{name:\"APPLE\", val:\"42\" }";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		REQUIRE( mp_apple_.isLocked() == true );
		valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( value == 21 );

		json   = "{name:\"APPLE\", locked:false}";
		result = modelDb_.fromJSON( json, &errorMsg );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
		REQUIRE( result == true );
		valid = mp_apple_.read( value );
		REQUIRE( ModelPoint::IS_VALID( valid ) == true );
		REQUIRE( value == 21 );
		REQUIRE( mp_apple_.isLocked() == false );
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
