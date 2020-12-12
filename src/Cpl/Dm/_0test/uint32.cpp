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
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "common.h"
#include <string.h>


////////////////////////////////////////////////////////////////////////////////

#define STRCMP(s1,s2)  (strcmp(s1,s2)==0)

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StaticInfo       info_mp_apple_( "APPLE" );
static Mp::Uint32       mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo       info_mp_orange_( "ORANGE" );
static Mp::Uint32       mp_orange_( modelDb_, info_mp_orange_, 32, false );

#define UNITS			"none"
#define DESCRIPTION		"my test description"
static MyStaticInfo     info_mp_cherry_( "CHERRY", UNITS, DESCRIPTION );
static Mp::Uint32       mp_cherry_( modelDb_, info_mp_cherry_, 666U );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "uint32-get", "[uint32-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-GET test" );
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
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s])", mpType ) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::Uint32-dec" ) == 0 );

    mpType = mp_orange_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s])", mpType ) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::Uint32-hex" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "uint32-export", "[uint32-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-EXPORT test" );
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
    seqNum = mp_apple_.write( 42 );
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
    seqNum = mp_apple_.write( 13 );
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
    mp_apple_.write( 666 );
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

TEST_CASE( "uint32-export2", "[uint32-export2]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-EXPORT test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //  Export/Import Buffer
    uint8_t streamBuffer[STREAM_BUFFER_SIZE];
    REQUIRE( mp_apple_.getExternalSize( true ) <= STREAM_BUFFER_SIZE );


    // Export...
    mp_apple_.setInvalid();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( mp_apple_.isLocked() == false );
    uint16_t seqNum  = mp_apple_.getSequenceNumber();
    uint16_t seqNum2 = mp_apple_.getSequenceNumber();
    size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize( true ) );
    REQUIRE( seqNum == seqNum2 );

    // Update the MP
    seqNum = mp_apple_.write( 42, ModelPoint::eLOCK );
    REQUIRE( seqNum == seqNum2 + 1 );
    uint32_t value;
    int8_t   valid;
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == 42 );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize( true ) );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( mp_apple_.isLocked() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Update the MP
    seqNum = mp_apple_.write( 13, ModelPoint::eLOCK );
    REQUIRE( seqNum == seqNum2 + 1 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == 13 );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize( true ) );
    REQUIRE( seqNum == seqNum2 );

    // set a new value AND invalidate the MP
    mp_apple_.write( 666, ModelPoint::eLOCK );
    seqNum = mp_apple_.setInvalid( ModelPoint::eUNLOCK );
    REQUIRE( seqNum == seqNum2 + 2 );
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( mp_apple_.isLocked() == false );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize( true ) );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( value == 13 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    1024

TEST_CASE( "uint32-toJSON" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-TOJSON test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    char string[MAX_STR_LENG + 1];
    bool truncated;
    mp_apple_.removeLock();

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
        uint16_t seqnum = mp_apple_.write( 127, ModelPoint::eUNLOCK );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["val"] == 127 );
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
        REQUIRE( doc["val"] == 127 );
    }

    SECTION( "HEX Value" )
    {
        uint16_t seqnum = mp_orange_.write( 0xBEEF, ModelPoint::eUNLOCK );
        mp_orange_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["val"] == "0xBEEF" );

    }

    SECTION( "HEX Value + Lock" )
    {
        uint16_t seqnum = mp_orange_.write( 0xDEAD, ModelPoint::eUNLOCK );
        mp_orange_.applyLock();
        mp_orange_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == true );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["val"] == "0xDEAD" );
    }

    SECTION("StaticInfo")
    {
        mp_cherry_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s])", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( strcmp( doc["info"]["units"], UNITS ) == 0 );
        REQUIRE( strcmp( doc["info"]["desc"], DESCRIPTION ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "uint32-fromJSON" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "UINT32-FROMJSON test" );
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
        const char* json = "{name:\"APPLE\", val:1234}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint32_t value;
        int8_t   valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 1234 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "Write value - error cases" )
    {
        const char* json   = "{name:\"APPLE\", val:\"abc\"}";
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
        json     = "{name:\"APPLE\", val:a123}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:123a}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        uint32_t value;
        int8_t valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 123 );
        REQUIRE( errorMsg == "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:2.2}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg =[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 2 );
        REQUIRE( errorMsg == "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:8589934591}";  // Exceed range of the uint32_t
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 0 );
        REQUIRE( errorMsg == "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"BOB\", invalid:1}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
    }

    SECTION( "Write value - HEX" )
    {
        const char* json = "{name:\"ORANGE\", val:\"0xBEEF\"}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        uint32_t value;
        int8_t   valid = mp_orange_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 0xBEEF );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_orange_ );

        errorMsg = "noerror";
        json     = "{name:\"ORANGE\", val:\"DEAD\"}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        valid = mp_orange_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 0xDEAD );
        REQUIRE( errorMsg == "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"ORANGE\", val:\"1234\"}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        valid = mp_orange_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == 0x1234 );

        errorMsg = "noerror";
        json     = "{name:\"ORANGE\", val:\"DTEAD\"}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
    }


    SECTION( "Set Invalid" )
    {
        seqNum = mp_apple_.write( 12 );
        const char* json = "{name:\"APPLE\", val:4321, invalid:1}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint32_t value;
        int8_t   valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) == false );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "lock..." )
    {
        const char* json = "{name:\"APPLE\", val:4321, locked:true}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        uint32_t value;
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

        json   = "{name:\"APPLE\", val:21, locked:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( value == 21 );

        json   = "{name:\"APPLE\", val:42 }";
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
