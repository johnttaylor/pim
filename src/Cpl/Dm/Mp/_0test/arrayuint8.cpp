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
#include "Cpl/Dm/Mp/ArrayUint8.h"
#include "common.h"
#include <string.h>


#define STRCMP(s1,s2)  (strcmp(s1,s2)==0)

////////////////////////////////////////////////////////////////////////////////

#define APPLE_NUM_ELEMENTS  5
#define ORANGE_NUM_ELEMENTS 3

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StaticInfo      info_mp_apple_( "APPLE" );
static Mp::ArrayUint8  mp_apple_( modelDb_, info_mp_apple_, APPLE_NUM_ELEMENTS );

static StaticInfo      info_mp_orange_( "ORANGE" );
static const uint8_t   orange_init_values[ORANGE_NUM_ELEMENTS] = { 3, 2, 1 };
static Mp::ArrayUint8  mp_orange_( modelDb_, info_mp_orange_, ORANGE_NUM_ELEMENTS, orange_init_values, false );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "arrayuint8-readwrite" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "ARRAYUINT8-READWRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Read
    uint8_t  value[ORANGE_NUM_ELEMENTS] = { 0xFF, 0xFF, 0xFF };
    int8_t   valid = mp_orange_.read( value, 2, 1 );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value[0] == 2 );
    REQUIRE( value[1] == 1 );
    REQUIRE( value[2] == 0xFF );
    uint16_t seqNum;
    valid = mp_apple_.read( value, 3, 0, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Write
    uint8_t  value2[APPLE_NUM_ELEMENTS] = { 50, 40, 30, 20, 10 };
    uint16_t seqNum2 = mp_apple_.write( value2 + 1, 3, ModelPoint::eNO_REQUEST, 1 );
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value2[0] == 0 );
    REQUIRE( value2[1] == 40 );
    REQUIRE( value2[2] == 30 );
    REQUIRE( value2[3] == 20 );
    REQUIRE( value2[4] == 0 );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Partial Write same value
    seqNum = mp_apple_.write( value2 + 1, 3, ModelPoint::eNO_REQUEST, 1 );
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value2[0] == 0 );
    REQUIRE( value2[1] == 40 );
    REQUIRE( value2[2] == 30 );
    REQUIRE( value2[3] == 20 );
    REQUIRE( value2[4] == 0 );
    REQUIRE( seqNum == seqNum2 );

    // Entire Write same value
    uint8_t  value3[APPLE_NUM_ELEMENTS] = { 0, 40, 30, 20, 0 };
    seqNum = mp_apple_.write( value3, APPLE_NUM_ELEMENTS );
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value2[0] == 0 );
    REQUIRE( value2[1] == 40 );
    REQUIRE( value2[2] == 30 );
    REQUIRE( value2[3] == 20 );
    REQUIRE( value2[4] == 0 );
    REQUIRE( seqNum == seqNum2 );

    // Read-Modify-Write with Lock
    RmwArrayUint8 callbackClient;
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_index          = 4;
    callbackClient.m_newValue       = 44;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    bool locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value2[0] == 0 );
    REQUIRE( value2[1] == 40 );
    REQUIRE( value2[2] == 30 );
    REQUIRE( value2[3] == 20 );
    REQUIRE( value2[4] == 44 );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    // Invalidate with Unlock
    mp_apple_.setInvalidState( 112, ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    valid = mp_apple_.getValidState();
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    REQUIRE( valid == 112 );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "arrayuint8-get" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "ARRAYUINT8-GET test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Gets...
    const char* name = mp_apple_.getName();
    REQUIRE( strcmp( name, "APPLE" ) == 0 );
    name = mp_orange_.getName();
    REQUIRE( strcmp( name, "ORANGE" ) == 0 );

    size_t s = mp_apple_.getSize();
    REQUIRE( s == APPLE_NUM_ELEMENTS * sizeof( uint8_t ) );
    s = mp_orange_.getSize();
    REQUIRE( s == ORANGE_NUM_ELEMENTS * sizeof( uint8_t ) );

    s = mp_apple_.getExternalSize();
    REQUIRE( s == APPLE_NUM_ELEMENTS * sizeof( uint8_t ) + sizeof( int8_t ) + sizeof( size_t ) * 2 + sizeof( uint8_t* ) );
    s = mp_orange_.getExternalSize();
    REQUIRE( s == ORANGE_NUM_ELEMENTS * sizeof( uint8_t ) + sizeof( int8_t ) + sizeof( size_t ) * 2 + sizeof( uint8_t* ) );

    const char* mpType = mp_apple_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::ArrayUint8-dec" ) == 0 );

    mpType = mp_orange_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("typeText: [%s]", mpType) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::ArrayUint8-hex" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "arrayuint8-export" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "ARRAYUINT8-EXPORT test" );
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
    uint8_t  value[APPLE_NUM_ELEMENTS] = { 50, 40, 30, 20, 10 };
    seqNum = mp_apple_.write( value, APPLE_NUM_ELEMENTS );
    REQUIRE( seqNum == seqNum2 + 1 );
    uint8_t  value2[APPLE_NUM_ELEMENTS] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    int8_t   valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value2[0] == 50 );
    REQUIRE( value2[1] == 40 );
    REQUIRE( value2[2] == 30 );
    REQUIRE( value2[3] == 20 );
    REQUIRE( value2[4] == 10 );


    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );
    REQUIRE( mp_apple_.getNumElements() == APPLE_NUM_ELEMENTS );

    // Read import value/state
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Update the MP
    uint8_t  value3[APPLE_NUM_ELEMENTS] = { 5, 4, 3, 2, 1 };
    seqNum = mp_apple_.write( value3, APPLE_NUM_ELEMENTS );
    REQUIRE( seqNum == seqNum2 + 1 );
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value2[0] == 5 );
    REQUIRE( value2[1] == 4 );
    REQUIRE( value2[2] == 3 );
    REQUIRE( value2[3] == 2 );
    REQUIRE( value2[4] == 1 );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Update and Invalidate the MP
    seqNum = mp_apple_.write( value, APPLE_NUM_ELEMENTS );
    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum == seqNum2 + 2 );
    REQUIRE( mp_apple_.isNotValid() == true );

    // Import...
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read import value/state
    valid = mp_apple_.read( value2, APPLE_NUM_ELEMENTS );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value2[0] == 5 );
    REQUIRE( value2[1] == 4 );
    REQUIRE( value2[2] == 3 );
    REQUIRE( value2[3] == 2 );
    REQUIRE( value2[4] == 1 );

    // Invalid import
    b = mp_orange_.importData( streamBuffer, sizeof( streamBuffer ) );
    REQUIRE( b == 0 );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    1024

TEST_CASE( "arrayuint8-toJSON" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "ARRAYUINT8-TOJSON test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    mp_apple_.setInvalid();
    mp_apple_.removeLock();
    mp_orange_.setInvalid();
    mp_orange_.removeLock();

    char string[MAX_STR_LENG + 1];
    bool truncated;

    SECTION( "Invalid" )
    {
        // Invalid (Default value)
        uint16_t seqnum = mp_apple_.setInvalid();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

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
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

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
        mp_apple_.setInvalidState( 100 );
        mp_apple_.applyLock();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["invalid"] == 100 );
        REQUIRE( doc["locked"] == true );
    }

    SECTION( "Hex Value" )
    {
        uint8_t  value[ORANGE_NUM_ELEMENTS] = { 51, 40, 31 };
        uint16_t seqnum = mp_orange_.write( value, ORANGE_NUM_ELEMENTS );
        mp_orange_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["val"]["start"] == 0 );
        REQUIRE( STRCMP( doc["val"]["elems"][0], "0x33" ) );
        REQUIRE( STRCMP( doc["val"]["elems"][2], "0x1F" ) );
    }

    SECTION( "Value" )
    {
        uint8_t  value[APPLE_NUM_ELEMENTS] = { 51, 40, 30, 20, 10 };
        uint16_t seqnum = mp_apple_.write( value, APPLE_NUM_ELEMENTS, ModelPoint::eUNLOCK );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["val"]["start"] == 0 );
        REQUIRE( doc["val"]["elems"][0] == 51 );
        REQUIRE( doc["val"]["elems"][4] == 10 );
    }

    SECTION( "Value + lock" )
    {
        uint8_t  value[APPLE_NUM_ELEMENTS] = { 52, 41, 30, 20, 11 };
        uint16_t seqnum = mp_apple_.write( value, APPLE_NUM_ELEMENTS, ModelPoint::eUNLOCK );
        mp_apple_.applyLock();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == true );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["val"]["start"] == 0 );
        REQUIRE( doc["val"]["elems"][0] == 52 );
        REQUIRE( doc["val"]["elems"][4] == 11 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "arrayuint8-fromJSON_", )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "ARRAYUINT8-FROMSJON_ test" );
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
        const char* json = "{name:\"APPLE\", val:{start:0,elems:[5,4,3,2,1]}}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint8_t  value[APPLE_NUM_ELEMENTS] = { 0x45, 0x45, 0x45, 0x45, 0x45 };
        int8_t   valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS, 0, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value[0] == 5 );
        REQUIRE( value[1] == 4 );
        REQUIRE( value[2] == 3 );
        REQUIRE( value[3] == 2 );
        REQUIRE( value[4] == 1 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "Write value - partial write" )
    {
        const char* json = "{name:\"APPLE\", val:{start:2,elems:[22,23]}}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint8_t  value[APPLE_NUM_ELEMENTS] = { 0x45, 0x45, 0x45, 0x45, 0x45 };
        int8_t   valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS, 0, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value[0] == 5 );
        REQUIRE( value[1] == 4 );
        REQUIRE( value[2] == 22 );
        REQUIRE( value[3] == 23 );
        REQUIRE( value[4] == 1 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "Write value - hex write" )
    {
        const char* json = "{name:\"ORANGE\", val:{start:0,elems:[\"10\",\"11\",\"12\"]}}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        uint8_t  value[ORANGE_NUM_ELEMENTS] = { 0x45, 0x45, 0x45 };
        int8_t   valid = mp_orange_.read( value, ORANGE_NUM_ELEMENTS, 0, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value[0] == 0x10 );
        REQUIRE( value[1] == 0x11 );
        REQUIRE( value[2] == 0x12 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_orange_ );

        json = "{name:\"ORANGE\", val:{elems:[\"1E\"]}}";
        result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_orange_.read( value, ORANGE_NUM_ELEMENTS, 0, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value[0] == 0x1E );
        REQUIRE( value[1] == 0x11 );
        REQUIRE( value[2] == 0x12 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_orange_ );
    }

    SECTION( "Write value - error cases" )
    {
        const char* json   = "{name:\"APPLE\", val:{start=0}";
        bool        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == false );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:{elems:[abc]}}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == false );
    }

    SECTION( "Set Invalid" )
    {
        uint8_t  value[APPLE_NUM_ELEMENTS] = { 0x45, 0x45, 0x45, 0x45, 0x45 };
        seqNum = mp_apple_.write( value, APPLE_NUM_ELEMENTS );
        const char* json = "{name:\"APPLE\", val:{start:2,elems:[22,23]}, invalid:1}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        int8_t   valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS, 0, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) == false );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "lock..." )
    {
        const char* json = "{name:\"APPLE\", val:{start:2,elems:[22,23]}, locked:true}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        uint8_t  value[APPLE_NUM_ELEMENTS] = { 0x45, 0x45, 0x45, 0x45, 0x45 };
        int8_t   valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp_apple_.isLocked() == true );
        REQUIRE( value[2] == 22 );
        REQUIRE( value[3] == 23 );

        json   = "{name:\"APPLE\", invalid:21, locked:false}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( mp_apple_.isLocked() == false );
        REQUIRE( mp_apple_.getValidState() == 21 );

        json   = "{name:\"APPLE\", val:{start:2,elems:[22,23]}, locked:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( value[2] == 22 );
        REQUIRE( value[3] == 23 );

        json   = "{name:\"APPLE\", val:{start:2,elems:[33,44]} }";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS );
        REQUIRE( value[2] == 22 );
        REQUIRE( value[3] == 23 );


        json   = "{name:\"APPLE\", locked:false}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("fromSJON errorMsg=[%s])", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( value[2] == 22 );
        REQUIRE( value[3] == 23 );
        REQUIRE( mp_apple_.isLocked() == false );
    }
#if 0 

    // Start with MP in the invalid state
    Cpl::Text::FString<MAX_STR_LENG> string;
    Cpl::Text::DString               errorMsg( "noerror", 1024 );
    mp_apple_.removeLock();
    mp_orange_.removeLock();
    mp_orange_.setInvalid();
    uint16_t    seqNum;
    uint16_t    seqNum2;
    const char* nextChar;
    uint8_t     value[APPLE_NUM_ELEMENTS] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    int8_t      valid;

    // Write Hex value- Pass case
    errorMsg = "noerror";
    nextChar = mp_orange_.fromString( "3:0:050403", 0, &errorMsg, &seqNum );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    valid = mp_orange_.read( value, ORANGE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( errorMsg == "noerror" );
    REQUIRE( value[0] == 5 );
    REQUIRE( value[1] == 4 );
    REQUIRE( value[2] == 3 );

    // Write Hex value- Fail case
    nextChar = mp_orange_.fromString( "3:0:5431", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar == 0 );
    valid = mp_orange_.read( value, ORANGE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( errorMsg != "noerror" );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("fromString FAILED: errorMsg=[%s]", errorMsg.getString()) );
    REQUIRE( value[0] == 5 );
    REQUIRE( value[1] == 4 );
    REQUIRE( value[2] == 3 );

    // Write value
    errorMsg = "noerror";
    seqNum   = mp_apple_.setInvalid();
    nextChar = mp_apple_.fromString( "5:0:1:2:3:4:5", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum + 1 );
    valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS, 0, &seqNum );
    REQUIRE( seqNum == seqNum2 );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( errorMsg == "noerror" );
    REQUIRE( value[0] == 1 );
    REQUIRE( value[1] == 2 );
    REQUIRE( value[2] == 3 );
    REQUIRE( value[3] == 4 );
    REQUIRE( value[4] == 5 );

    // Set Invalid
    nextChar = mp_apple_.fromString( "?", 0, 0, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum + 1 );
    REQUIRE( mp_apple_.isNotValid() == true );

    // Set Invalid with lock
    REQUIRE( mp_apple_.isLocked() == false );
    nextChar = mp_orange_.fromString( "!?" );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == true );
    REQUIRE( mp_orange_.isLocked() == true );

    // Write with Lock
    nextChar = mp_apple_.fromString( "!5:0:11:21:31:41:51", 0, 0, &seqNum );
    valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 + 1 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( value[0] == 11 );
    REQUIRE( value[1] == 21 );
    REQUIRE( value[2] == 31 );
    REQUIRE( value[3] == 41 );
    REQUIRE( value[4] == 51 );


    // Write while locked
    errorMsg = "noerror";
    nextChar = mp_apple_.fromString( "5:0:12:22:32:42:52", 0, &errorMsg, &seqNum2 );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( mp_apple_.isLocked() == true );
    valid = mp_apple_.read( value, APPLE_NUM_ELEMENTS, 0, &seqNum );
    REQUIRE( seqNum2 == seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( mp_apple_.isLocked() == true );
    REQUIRE( errorMsg == "noerror" );
    REQUIRE( value[0] == 11 );
    REQUIRE( value[1] == 21 );
    REQUIRE( value[2] == 31 );
    REQUIRE( value[3] == 41 );
    REQUIRE( value[4] == 51 );

    // Write with unlock
    REQUIRE( mp_orange_.isLocked() == true );
    nextChar = mp_orange_.fromString( "^3:0:0D1721", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    valid = mp_orange_.read( value, ORANGE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value[0] == 13 );
    REQUIRE( value[1] == 23 );
    REQUIRE( value[2] == 33 );


    // Just lock
    nextChar = mp_orange_.fromString( "!", 0, &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == '\0' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == true );
    valid = mp_orange_.read( value, ORANGE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value[0] == 13 );
    REQUIRE( value[1] == 23 );
    REQUIRE( value[2] == 33 );


    // Test termination characters
    nextChar = mp_orange_.fromString( "^3:0:1D2722,", ": ,;", &errorMsg );
    REQUIRE( nextChar != 0 );
    REQUIRE( *nextChar == ',' );
    REQUIRE( mp_orange_.isNotValid() == false );
    REQUIRE( mp_orange_.isLocked() == false );
    valid =  mp_orange_.read( value, ORANGE_NUM_ELEMENTS );
    REQUIRE( ModelPoint::IS_VALID( valid ) );
    REQUIRE( value[0] == 0x1d );
    REQUIRE( value[1] == 0x27 );
    REQUIRE( value[2] == 0x22 );
#endif

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
///////////////////////////////////////////////////////////////////////////////

static Cpl::Dm::MailboxServer     t1Mbox_;

TEST_CASE( "arrayuint8-observer" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "ARRAYUINT8-OBSERVER test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    ViewerArrayUint8 viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

    // Open, write a value, wait for Viewer to see the change, then close
    mp_apple_.removeLock();
    viewer1.open();
    uint8_t  value2[APPLE_NUM_ELEMENTS] = { 50, 140, 30, 20, 10 };
    uint16_t seqNum                     = mp_apple_.write( value2 + 1, 3, ModelPoint::eNO_REQUEST, 1 );
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
