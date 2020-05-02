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
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Storm/Dm/MpCycleInfo.h"
#include "Cpl/Math/real.h"
#include "common.h"
#include <string.h>



using namespace Storm::Dm;
static Cpl::Dm::MailboxServer     t1Mbox_;

#define STRCMP(a,b)               (strcmp((a),(b)) == 0)

/////////////////////////////////////////////////////////////////
namespace {
class Rmw : public MpCycleInfo::Client
{
public:
    ///
    int                                         m_callbackCount;
    ///
    Cpl::Dm::ModelPoint::RmwCallbackResult_T    m_returnResult;
    ///
    uint32_t                                    m_onTime;

public:
    ///
    Rmw() :m_callbackCount( 0 ), m_returnResult( Cpl::Dm::ModelPoint::eNO_CHANGE ), m_onTime( 0 ) {}
    ///
    Cpl::Dm::ModelPoint::RmwCallbackResult_T callback( Storm::Type::CycleInfo_T& data, int8_t validState ) noexcept
    {
        m_callbackCount++;
        if ( m_returnResult != Cpl::Dm::ModelPoint::eNO_CHANGE )
        {
            data.onTime = m_onTime;
        }
        return m_returnResult;
    }
};
}; // end Anonymous namespace

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::StaticInfo      info_mp_apple_( "APPLE" );
static MpCycleInfo              mp_apple_( modelDb_, info_mp_apple_ );

static Cpl::Dm::StaticInfo      info_mp_orange_( "ORANGE" );
static MpCycleInfo              mp_orange_( modelDb_, info_mp_orange_ );

static bool compare( Storm::Type::CycleInfo_T d, Storm::Type::CycleStatus mode=Storm::Type::CycleStatus::eOFF, uint32_t onTime=0, uint32_t offTime=0, Cpl::System::ElapsedTime::Precision_T beginOnTime= { 0,0 }, Cpl::System::ElapsedTime::Precision_T beginOffTime= { 0,0 } )
{
    return d.mode == mode && d.onTime == onTime && d.offTime == offTime && d.beginOnTime == beginOnTime && d.beginOffTime == beginOffTime;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MP CycleInfo" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "ReadWrite" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "READWRITE test" );

        // Read
        Storm::Type::CycleInfo_T value;
        uint16_t                 seqNum;
        int8_t                   valid = mp_orange_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value ) == true );
        valid = mp_apple_.read( value, &seqNum );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );

        // Write
        uint16_t seqNum2 = mp_apple_.setMode( Storm::Type::CycleStatus::eON_CYCLE );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Write
        seqNum = mp_apple_.setOnTime( 1 );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE, 1 ) );
        REQUIRE( seqNum == seqNum2 + 1 );

        // Write
        seqNum2 = mp_apple_.setOffTime( 2 );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE, 1, 2 ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Write
        seqNum = mp_apple_.setBeginOnTime( { 3,1 } );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE, 1, 2, { 3,1 } ) );
        REQUIRE( seqNum == seqNum2 + 1 );

        // Write
        seqNum2 = mp_apple_.setBeginOffTime( { 4,2 } );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE, 1, 2, { 3,1 }, { 4, 2 } ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read-Modify-Write with Lock
        Rmw callbackClient;
        callbackClient.m_callbackCount  = 0;
        callbackClient.m_onTime         = 111;
        callbackClient.m_returnResult   = Cpl::Dm::ModelPoint::eCHANGED;
        mp_apple_.readModifyWrite( callbackClient, Cpl::Dm::ModelPoint::eLOCK );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        bool locked = mp_apple_.isLocked();
        REQUIRE( locked == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE, 111, 2, { 3,1 }, { 4, 2 } ) );
        REQUIRE( callbackClient.m_callbackCount == 1 );

        // Invalidate with Unlock
        mp_apple_.setInvalidState( 112, Cpl::Dm::ModelPoint::eUNLOCK );
        REQUIRE( mp_apple_.isNotValid() == true );
        valid = mp_apple_.getValidState();
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == false );
        REQUIRE( valid == 112 );
    }

    SECTION( "get" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "GET test" );

        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );
        name = mp_orange_.getName();
        REQUIRE( strcmp( name, "ORANGE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( Storm::Type::CycleInfo_T ) );
        s = mp_orange_.getSize();
        REQUIRE( s == sizeof( Storm::Type::CycleInfo_T ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( Storm::Type::CycleInfo_T ) + sizeof( int8_t ) );
        s = mp_orange_.getExternalSize();
        REQUIRE( s == sizeof( Storm::Type::CycleInfo_T ) + sizeof( int8_t ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s])", mpType ) );
        REQUIRE( strcmp( mpType, "Storm::Dm::MpCycleInfo" ) == 0 );
    }

#define STREAM_BUFFER_SIZE  100

    SECTION( "export/import" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "EXPORT/IMPORT test" );

        //  Export/Import Buffer
        uint8_t streamBuffer[STREAM_BUFFER_SIZE];
        REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );


        // Export...
        mp_apple_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == true );
        uint16_t seqNum  = mp_apple_.getSequenceNumber();
        uint16_t seqNum2 = mp_apple_.getSequenceNumber();
        size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // Update the MP
        seqNum = mp_apple_.setMode( Storm::Type::CycleStatus::eTRANSITIONING_DOWN );
        REQUIRE( seqNum == seqNum2 + 1 );
        Storm::Type::CycleInfo_T value;
        int8_t           valid;
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eTRANSITIONING_DOWN ) == true );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == false );

        // Update the MP
        seqNum = mp_apple_.setMode( Storm::Type::CycleStatus::eTRANSITIONING_UP );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eTRANSITIONING_UP ) == true );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == false );
        b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // Set and new value AND invalidate the MP
        mp_apple_.setMode( Storm::Type::CycleStatus::eON_CYCLE );
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
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( compare( value, Storm::Type::CycleStatus::eTRANSITIONING_UP ) == true );
    }

    ///////////////////////////////////////////////////////////////////////////////


    SECTION( "observer" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "observer TEST" );

        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
        AsyncClient<MpCycleInfo> viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

        // Open, write a value, wait for Viewer to see the change, then close
        mp_apple_.removeLock();
        viewer1.open();
        uint16_t seqNum = mp_apple_.setOnTime( 1113 );
        Cpl::System::Thread::wait();
        viewer1.close();
        REQUIRE( viewer1.m_lastSeqNumber == seqNum );

        // Shutdown threads
        t1Mbox_.pleaseStop();
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        Cpl::System::Thread::destroy( *t1 );
    }

    ///////////////////////////////////////////////////////////////////////////////
#define MAX_STR_LENG    1024

    SECTION( "toJSON" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "TOJSON test" );
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
            Storm::Type::CycleStatus mode  = Storm::Type::CycleStatus::eTRANSITIONING_DOWN;
            Storm::Type::CycleInfo_T value = { {1,1}, {2,2}, 3, 4, mode };
            uint16_t seqnum = mp_apple_.write( value, Cpl::Dm::ModelPoint::eUNLOCK );
            mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

            StaticJsonDocument<1024> doc;
            DeserializationError err = deserializeJson( doc, string );
            REQUIRE( err == DeserializationError::Ok );
            REQUIRE( doc["seqnum"] == seqnum );
            REQUIRE( doc["locked"] == false );
            REQUIRE( doc["invalid"] == 0 );
            JsonObject val = doc["val"];
            REQUIRE( val["onTimeMsec"] == 3 );
            REQUIRE( val["offTimeMsec"] == 4 );
            REQUIRE( Cpl::Math::areDoublesEqual( val["beginOnTimeSec"], 1.001 ) );
            REQUIRE( Cpl::Math::areDoublesEqual( val["beginOffTimeSec"], 2.002 ) );
            const char* actualEnum   = val["mode"];
            const char* expectedEnum = mode._to_string();
            REQUIRE( STRCMP( actualEnum, expectedEnum ) );
        }

        SECTION( "Value + Lock" )
        {
            Storm::Type::CycleStatus mode  = Storm::Type::CycleStatus::eTRANSITIONING_DOWN;
            mp_apple_.applyLock();
            mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

            StaticJsonDocument<1024> doc;
            DeserializationError err = deserializeJson( doc, string );
            REQUIRE( err == DeserializationError::Ok );
            REQUIRE( doc["locked"] == true );
            REQUIRE( doc["invalid"] == 0 );
            JsonObject val = doc["val"];
            REQUIRE( val["onTimeMsec"] == 3 );
            REQUIRE( val["offTimeMsec"] == 4 );
            REQUIRE( Cpl::Math::areDoublesEqual( val["beginOnTimeSec"], 1.001 ) );
            REQUIRE( Cpl::Math::areDoublesEqual( val["beginOffTimeSec"], 2.002 ) );
            const char* actualEnum   = val["mode"];
            const char* expectedEnum = mode._to_string();
            REQUIRE( STRCMP( actualEnum, expectedEnum ) );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    SECTION( "fromJSON" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "fromJSON test" );

        // Start with MP in the invalid state
        Cpl::Text::FString<MAX_STR_LENG> string;
        Cpl::Text::DString               errorMsg( "noerror", 1024 );
        mp_apple_.removeLock();
        mp_orange_.removeLock();
        mp_orange_.setInvalid();
        uint16_t seqNum = mp_apple_.setInvalid();
        uint16_t seqNum2;
        Cpl::Dm::ModelPoint* mp;

        SECTION( "Write value" )
        {
            const char* json = "{name:\"APPLE\", val:{onTimeMsec:6, offTimeMsec:5, beginOnTimeSec:4.5, beginOffTimeSec:3.3, mode:\"eON_CYCLE\" }}";
            bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum2 == seqNum + 1 );
            Storm::Type::CycleInfo_T value;
            int8_t           valid = mp_apple_.read( value, &seqNum );
            REQUIRE( seqNum == seqNum2 );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) );
            REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE, 6, 5, { 4,500 }, { 3,300 } ) == true );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp == &mp_apple_ );
        }

        SECTION( "Write value - error cases" )
        {
            Storm::Type::CycleInfo_T value;
            value.onTime  = 1;
            value.offTime = 2;
            uint16_t seqNum = mp_apple_.write( value );
            const char* json   = "{name:\"APPLE\", val:\"abc\"}";
            bool        result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=(%s)", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum == mp_apple_.getSequenceNumber() ); // JSON parsing 'passed' -->but NO CHANGE to the actual MP.

            errorMsg = "noerror";
            json     = "{name:\"APPLE\"}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=(%s)", errorMsg.getString() ) );
            REQUIRE( result == false );
            REQUIRE( errorMsg != "noerror" );

            errorMsg = "noerror";
            json     = "{namex:\"APPLE\"}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=(%s)", errorMsg.getString() ) );
            REQUIRE( result == false );
            REQUIRE( errorMsg != "noerror" );

            errorMsg = "noerror";
            json     = "{name:\"APPLE\", val:a123}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=(%s)", errorMsg.getString() ) );
            REQUIRE( result == false );
            REQUIRE( errorMsg != "noerror" );

            errorMsg = "noerror";
            json     = "{name:\"APPLE\", val:{}}";
            seqNum   = mp_apple_.getSequenceNumber();
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=(%s)", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum == mp_apple_.getSequenceNumber() ); // JSON parsing 'passed' -->but NO CHANGE to the actual MP.

            errorMsg = "noerror";
            json     = "{name:\"APPLE\", val:{priAlarm:123}}";
            seqNum   = mp_apple_.getSequenceNumber();
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg =(%s)", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum == mp_apple_.getSequenceNumber() ); // JSON parsing 'passed' -->but NO CHANGE to the actual MP.

            errorMsg = "noerror";
            json     = "{name:\"BOB\", invalid:1}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=(%s)", errorMsg.getString() ) );
            REQUIRE( result == false );
        }

        SECTION( "Set Invalid" )
        {
            uint16_t seqNum = mp_apple_.setOnTime( 666 );
            const char* json = "{name:\"APPLE\", val:{onTimeMsec:5}, invalid:1}";
            bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum2 == seqNum + 1 );
            Storm::Type::CycleInfo_T value;
            int8_t           valid = mp_apple_.read( value, &seqNum );
            REQUIRE( seqNum == seqNum2 );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == false );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp == &mp_apple_ );
        }

        SECTION( "lock..." )
        {
            const char* json = "{name:\"APPLE\", val:{mode:\"eOFF_CYCLE\"}, locked:true}";
            bool result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            Storm::Type::CycleInfo_T value;
            int8_t           valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp_apple_.isLocked() == true );
            REQUIRE( compare( value, Storm::Type::CycleStatus::eOFF_CYCLE ) == true );

            json   = "{name:\"APPLE\", invalid:21, locked:false}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isNotValid() == true );
            REQUIRE( mp_apple_.isLocked() == false );
            REQUIRE( mp_apple_.getValidState() == 21 );

            json   = "{name:\"APPLE\", val:{mode:\"eON_CYCLE\"}, locked:true}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isLocked() == true );
            valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE ) == true );

            json   = "{name:\"APPLE\", val:{mode:\"eOFF\"} }";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isLocked() == true );
            valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE ) == true );

            json   = "{name:\"APPLE\", locked:false}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( compare( value, Storm::Type::CycleStatus::eON_CYCLE ) == true );
            REQUIRE( mp_apple_.isLocked() == false );
        }
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

