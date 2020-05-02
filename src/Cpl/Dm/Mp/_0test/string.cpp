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
#include "Cpl/Math/real.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/String.h"
#include "common.h"
#include <string.h>

#define STRCMP(s1,s2)  (strcmp(s1,s2)==0)


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
#define APPLE_MAX_SIZE  10
static StaticInfo      info_mp_apple_( "APPLE" );
static Mp::String      mp_apple_( modelDb_, info_mp_apple_, APPLE_MAX_SIZE );

#define ORANGE_MAX_SIZE 15
static StaticInfo      info_mp_orange_( "ORANGE" );
static Mp::String      mp_orange_( modelDb_, info_mp_orange_, ORANGE_MAX_SIZE, "bobs yours uncle (should get truncated)" );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "string-readwrite", "[string-readwrite]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-READWRITE test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Read1
    char             myDataMemory[5];
    Mp::String::Data myData ={ myDataMemory, 0, sizeof( myDataMemory ) - 1 };
    uint16_t         seqNum;
    int8_t           valid = mp_orange_.read( myData );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( strcmp( myData.stringPtr, "bobs" ) == 0 );

    // Read2
    Cpl::Text::FString<32> value;
    valid = mp_orange_.read( value, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "bobs yours uncl" );
    valid = mp_apple_.read( value, &seqNum );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );

    // Write1
    uint16_t seqNum2 = mp_apple_.write( "-10.1234F" );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "-10.1234F" );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Write2
    seqNum = mp_apple_.write( "-10.1234F", 3 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "-10" );
    REQUIRE( seqNum == seqNum2 + 1 );

    // Write3
    Mp::String::Data myData2 ={ (char*)"bob was here today", 18, 18 };
    seqNum2 = mp_apple_.write( myData2 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == "bob was he" );
    REQUIRE( seqNum + 1 == seqNum2 );

    // Read-Modify-Write with Lock (with too long of string)
    RmwString callbackClient;
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_valueToWrite   = "Hello bob.!";
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    bool locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == "Hello bob." );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    // Read-Modify-Write with Lock 
    mp_apple_.removeLock();
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_valueToWrite   = "Hello bob!";
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == "Hello bob!" );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    // Invalidate with Unlock
    mp_apple_.setInvalidState( 112, ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    valid = mp_apple_.getValidState();
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    REQUIRE( valid == 112 );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "string-get", "[string-get]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-GET test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Gets...
    const char* name = mp_apple_.getName();
    REQUIRE( strcmp( name, "APPLE" ) == 0 );
    name = mp_orange_.getName();
    REQUIRE( strcmp( name, "ORANGE" ) == 0 );

    size_t s = mp_apple_.getSize();
    REQUIRE( s == APPLE_MAX_SIZE );
    s = mp_orange_.getSize();
    REQUIRE( s == ORANGE_MAX_SIZE );

    s = mp_apple_.getExternalSize();
    REQUIRE( s == APPLE_MAX_SIZE + sizeof( int8_t ) + sizeof( Cpl::Dm::Mp::String::Data ) + 1 );
    s = mp_orange_.getExternalSize();
    REQUIRE( s == ORANGE_MAX_SIZE + sizeof( int8_t ) + sizeof( Cpl::Dm::Mp::String::Data ) + 1 );

    const char* mpType = mp_apple_.getTypeAsText();
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s])", mpType ) );
    REQUIRE( strcmp( mpType, "Cpl::Dm::Mp::String" ) == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE  100

TEST_CASE( "string-export", "[string-export]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-EXPORT test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //  Export/Import Buffer
    uint8_t streamBuffer[STREAM_BUFFER_SIZE];
    REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );


    // Export invalid...
    REQUIRE( mp_apple_.isNotValid() == true );
    uint16_t seqNum  = mp_apple_.getSequenceNumber();
    uint16_t seqNum2 = mp_apple_.getSequenceNumber();
    size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Update the MP
    seqNum = mp_apple_.write( "-42.14159F" );
    REQUIRE( seqNum == seqNum2 + 1 );
    Cpl::Text::FString<32> value;
    int8_t                 valid;
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == "-42.14159F" );

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
    seqNum = mp_apple_.write( "13.99F" );
    REQUIRE( seqNum == seqNum2 + 1 );
    valid = mp_apple_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( value == "13.99F" );

    // Export...
    REQUIRE( mp_apple_.isNotValid() == false );
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
    REQUIRE( b != 0 );
    REQUIRE( b == mp_apple_.getExternalSize() );
    REQUIRE( seqNum == seqNum2 );

    // Set a new value AND invalidate the MP
    mp_apple_.write( "bob's here" );
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
    REQUIRE( value == "13.99F" );
    char memData[128];
    Cpl::Dm::Mp::String::Data dataValue ={ memData, 0, sizeof( memData ) - 1 };
    valid = mp_apple_.read( dataValue );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( dataValue.stringLen == strlen( "13.99F" ) );

    // Unwise - but valid Export/Import
    b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ) );
    REQUIRE( b > 0 );
    b = mp_orange_.importData( streamBuffer, sizeof( streamBuffer ) );
    REQUIRE( b > 0 );   // This will work because orange has large maxLen than apple
    REQUIRE( mp_orange_.getSize() == ORANGE_MAX_SIZE );

    // Invalid Export/Import
    b = mp_orange_.exportData( streamBuffer, sizeof( streamBuffer ) );
    REQUIRE( b > 0 );
    b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ) );
    REQUIRE( b == 0 );   
    REQUIRE( mp_apple_.getSize() == APPLE_MAX_SIZE );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


///////////////////////////////////////////////////////////////////////////////
static Cpl::Dm::MailboxServer     t1Mbox_;

TEST_CASE( "string-observer", "[string-observer]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-OBSERVER test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    ViewerString viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

    // Open, write a value, wait for Viewer to see the change, then close
    mp_apple_.removeLock();
    viewer1.open();
    uint16_t seqNum = mp_apple_.write( "33" );
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

TEST_CASE( "string-toJSON" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-TOJSON test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    char string[MAX_STR_LENG + 1];
    bool truncated;


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
        uint16_t seqnum = mp_apple_.write( "hi bob", ModelPoint::eUNLOCK );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["invalid"] == 0 );
        REQUIRE( doc["maxlen"] == mp_apple_.getSize() );
        REQUIRE( STRCMP( doc["val"], "hi bob" ) );
    }

    SECTION( "Value/terse" )
    {
        // Invalid (Default value)
        mp_apple_.write( "hi bob", ModelPoint::eUNLOCK );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: terse [%s])", string ) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( STRCMP( doc["name"], "APPLE" ) );
        REQUIRE( doc["type"].as<char*>() == 0 );
        REQUIRE( doc["seqnum"].as<char*>() == 0 );
        REQUIRE( doc["locked"].as<char*>() == 0 );
        REQUIRE( STRCMP( doc["val"], "hi bob" ) );
        REQUIRE( doc["invalid"] == 0 );
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
        REQUIRE( STRCMP( doc["val"], "hi bob" ) );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "string-fromJSON" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "STRING-FROMJSON test" );
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
    Cpl::Text::FString<128> value;

    SECTION( "Write value" )
    {
        const char* json = "{name:\"APPLE\", val:\"i am here\"}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        int8_t   valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) );
        REQUIRE( value == "i am here" );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "Write value - error cases" )
    {
        const char* json   = "{name:\"APPLE\", val:true}";
        bool        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\"}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = "{namex:\"APPLE\"}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:123}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:2.2e10}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg =[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"BOB\", invalid:1}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = "{name:\"APPLE\", val:\"this is too long of string for the model point\"}";
        result   = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );
    }

    SECTION( "Set Invalid" )
    {
        seqNum = mp_apple_.write( "uncle bob was here" );
        const char* json = "{name:\"APPLE\", val:\"ePIGS\", invalid:1}";
        bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        int8_t valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( ModelPoint::IS_VALID( valid ) == false );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "lock..." )
    {
        const char* json = "{name:\"APPLE\", val:\"ePIGS\", locked:true}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        int8_t   valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp_apple_.isLocked() == true );
        REQUIRE( value == "ePIGS" );

        json   = "{name:\"APPLE\", invalid:21, locked:false}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( mp_apple_.isLocked() == false );
        REQUIRE( mp_apple_.getValidState() == 21 );

        json   = "{name:\"APPLE\", val:\"eDOGS\", locked:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( value == "eDOGS" );

        json   = "{name:\"APPLE\", val:\"eCATS\" }";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( value == "eDOGS" );

        json   = "{name:\"APPLE\", locked:false}";
        result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( value == "eDOGS" );
        REQUIRE( mp_apple_.isLocked() == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
