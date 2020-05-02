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
#include "Cpl/Math/real.h"
#include "Storm/Dm/MpSystemConfig.h"
#include "common.h"
#include <string.h>



using namespace Storm::Dm;
static Cpl::Dm::MailboxServer     t1Mbox_;

#define STRCMP(a,b)               (strcmp(a,b) == 0)

/////////////////////////////////////////////////////////////////
namespace {
class Rmw : public MpSystemConfig::Client
{
public:
    ///
    int                                         m_callbackCount;
    ///
    Cpl::Dm::ModelPoint::RmwCallbackResult_T    m_returnResult;
    ///
    int                                         m_newTotalStages;

public:
    ///
    Rmw() :m_callbackCount( 0 ), m_returnResult( Cpl::Dm::ModelPoint::eNO_CHANGE ), m_newTotalStages( 0 ) {}
    ///
    Cpl::Dm::ModelPoint::RmwCallbackResult_T callback( Storm::Type::SystemConfig_T& data, int8_t validState ) noexcept
    {
        m_callbackCount++;
        if ( m_returnResult != Cpl::Dm::ModelPoint::eNO_CHANGE )
        {
            data.totalStages = m_newTotalStages;
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
static MpSystemConfig           mp_apple_( modelDb_, info_mp_apple_ );

static Cpl::Dm::StaticInfo      info_mp_orange_( "ORANGE" );
static MpSystemConfig           mp_orange_( modelDb_, info_mp_orange_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MP SystemConfig" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "ReadWrite" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "READWRITE test" );

        // Initial valid state
        uint16_t                    seqNum;
        Storm::Type::SystemConfig_T value;
        int8_t                      valid = mp_apple_.read( value, &seqNum );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );

        // Write/Read
        Storm::Type::SystemConfig_T expectedValue = { {{0.0F, 1.0F, 2, 3, 4, 5, Storm::Type::Cph::e5CPH}, }, Storm::Type::OperatingMode::eHEATING, Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, 1, 4 };
        uint16_t seqNum2 = mp_apple_.write( expectedValue );
        valid            = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( memcmp( &value, &expectedValue, sizeof( value ) ) == 0 );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Write
        expectedValue = { {{0.0F, 1.0F, 3, 4, 5, 6, Storm::Type::Cph::e2CPH}, }, Storm::Type::OperatingMode::eCOOLING, Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 1, 1, 2, 10, 1.5F, 2.5F, 3.5F };
        seqNum = mp_apple_.write( expectedValue );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( memcmp( &value, &expectedValue, sizeof( value ) ) == 0 );
        REQUIRE( seqNum == seqNum2 + 1 );

        // Read-Modify-Write with Lock
        Rmw callbackClient;
        callbackClient.m_callbackCount  = 0;
        callbackClient.m_newTotalStages = 3;
        callbackClient.m_returnResult   = Cpl::Dm::ModelPoint::eCHANGED;
        mp_apple_.readModifyWrite( callbackClient, Cpl::Dm::ModelPoint::eLOCK );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        bool locked = mp_apple_.isLocked();
        REQUIRE( locked == true );
        REQUIRE( value.totalStages == 3 );
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
        REQUIRE( s == sizeof( Storm::Type::SystemConfig_T ) );
        s = mp_orange_.getSize();
        REQUIRE( s == sizeof( Storm::Type::SystemConfig_T ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( Storm::Type::SystemConfig_T ) + sizeof( int8_t ) );
        s = mp_orange_.getExternalSize();
        REQUIRE( s == sizeof( Storm::Type::SystemConfig_T ) + sizeof( int8_t ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s])", mpType ) );
        REQUIRE( strcmp( mpType, "Storm::Dm::MpSystemConfig" ) == 0 );
    }

#define STREAM_BUFFER_SIZE  1000

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
        Storm::Type::SystemConfig_T value;
        Storm::Type::SystemConfig_T expectedValue = { {{0.0F, 1.0F, 2, 3}, }, Storm::Type::OperatingMode::eHEATING, Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, 1, 4 };
        seqNum = mp_apple_.write( expectedValue );
        REQUIRE( seqNum == seqNum2 + 1 );
        int8_t           valid;
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( memcmp( &value, &expectedValue, sizeof( value ) ) == 0 );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( expectedValue );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == false );

        // Update the MP
        expectedValue = { {{0.0F, 1.0F, 3, 4}, }, Storm::Type::OperatingMode::eHEATING, Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, 2, 5 };
        seqNum = mp_apple_.write( expectedValue );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( memcmp( &value, &expectedValue, sizeof( value ) ) == 0 );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == false );
        b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // Set and new value AND invalidate the MP
        Storm::Type::SystemConfig_T newValue = { {{1.0F, 1.0F, 6, 7}, }, Storm::Type::OperatingMode::eCOOLING, Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, 2, 8 };
        mp_apple_.write( newValue );
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
        REQUIRE( memcmp( &value, &expectedValue, sizeof( value ) ) == 0 );
    }

    ///////////////////////////////////////////////////////////////////////////////


    SECTION( "observer" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "observer TEST" );

        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
        AsyncClient<MpSystemConfig> viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

        // Open, write a value, wait for Viewer to see the change, then close
        mp_apple_.removeLock();
        viewer1.open();
        Storm::Type::SystemConfig_T expectedValue = { {{1.0F, 1.0F, 2,3}, }, Storm::Type::OperatingMode::eHEATING, Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, 2, 4 };
        uint16_t seqNum = mp_apple_.write( expectedValue );
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

            StaticJsonDocument<2048> doc;
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

            StaticJsonDocument<2048> doc;
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

            StaticJsonDocument<2048> doc;
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

            StaticJsonDocument<2048> doc;
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

            StaticJsonDocument<2048> doc;
            DeserializationError err = deserializeJson( doc, string );
            REQUIRE( err == DeserializationError::Ok );
            REQUIRE( doc["invalid"] == 100 );
            REQUIRE( doc["locked"] == true );
        }

        SECTION( "Value" )
        {
            Storm::Type::SystemConfig_T expectedValue = { {{1.5F, 10.25F, 2, 3, 4, 5, Storm::Type::Cph::e4CPH}, }, Storm::Type::OperatingMode::eCOOLING, Storm::Type::OduType::eAC, Storm::Type::IduType::eFURNACE, 0, 1, 2, 4, 5.5, 6.5, 7.5 };
            uint16_t seqnum = mp_apple_.write( expectedValue, Cpl::Dm::ModelPoint::eUNLOCK );
            mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s]", string ) );

            StaticJsonDocument<2048> doc;
            DeserializationError err = deserializeJson( doc, string );
            REQUIRE( err == DeserializationError::Ok );
            REQUIRE( doc["seqnum"] == seqnum );
            REQUIRE( doc["locked"] == false );
            REQUIRE( doc["invalid"] == 0 );
            JsonObject val = doc["val"];
            REQUIRE( STRCMP( val["opMode"], "eCOOLING" ) );
            REQUIRE( STRCMP( val["iduType"], "eFURNACE" ) );
            REQUIRE( STRCMP( val["oduType"], "eAC" ) );
            REQUIRE( val["numCompStages"] == 0 );
            REQUIRE( val["numIdStages"] == 1 );
            REQUIRE( val["totalStages"] == 2 );
            REQUIRE( val["fanCont"] == 4 );
            REQUIRE( Cpl::Math::areDoublesEqual( val["gain"], 5.5 ) );
            REQUIRE( Cpl::Math::areDoublesEqual( val["reset"], 6.5 ) );
            REQUIRE( Cpl::Math::areDoublesEqual( val["maxPv"], 7.5 ) );
            REQUIRE( val["stages"][0]["stage"] == 1 );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][0]["lower"].as<double>() ), 1.5F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][0]["upper"].as<double>() ), 10.25F ) );
            REQUIRE( val["stages"][0]["minBlower"] == 2 );
            REQUIRE( val["stages"][0]["maxBlower"] == 3 );
            REQUIRE( val["stages"][0]["minOn"] == 4 );
            REQUIRE( val["stages"][0]["minOff"] == 5 );
            REQUIRE( STRCMP( val["stages"][0]["cph"], "e4CPH" ) );
            REQUIRE( val["stages"][1]["stage"] == 2 );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][1]["lower"].as<double>() ), 0.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][1]["upper"].as<double>() ), 0.0F ) );
            REQUIRE( val["stages"][1]["minBlower"] == 0 );
            REQUIRE( val["stages"][1]["maxBlower"] == 0 );
            REQUIRE( val["stages"][1]["minOn"] == 0 );
            REQUIRE( val["stages"][1]["minOff"] == 0 );
            REQUIRE( STRCMP( val["stages"][1]["cph"], "e2CPH") );
        }

        SECTION( "Value + Lock" )
        {
            mp_apple_.applyLock();
            mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s]", string ) );

            StaticJsonDocument<2048> doc;
            DeserializationError err = deserializeJson( doc, string );
            REQUIRE( err == DeserializationError::Ok );
            REQUIRE( doc["locked"] == true );
            REQUIRE( doc["invalid"] == 0 );
            JsonObject val = doc["val"];
            REQUIRE( val["numCompStages"] == 0 );
            REQUIRE( val["numIdStages"] == 1 );
            REQUIRE( val["totalStages"] == 2 );
            REQUIRE( val["fanCont"] == 4 );
            REQUIRE( Cpl::Math::areDoublesEqual( val["gain"], 5.5 ) );
            REQUIRE( Cpl::Math::areDoublesEqual( val["reset"], 6.5 ) );
            REQUIRE( Cpl::Math::areDoublesEqual( val["maxPv"], 7.5 ) );
            REQUIRE( val["stages"][0]["stage"] == 1 );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][0]["lower"].as<double>() ), 1.5F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][0]["upper"].as<double>() ), 10.25F ) );
            REQUIRE( val["stages"][0]["minBlower"] == 2 );
            REQUIRE( val["stages"][0]["maxBlower"] == 3 );
            REQUIRE( val["stages"][0]["minOn"] == 4 );
            REQUIRE( val["stages"][0]["minOff"] == 5 );
            REQUIRE( STRCMP( val["stages"][0]["cph"], "e4CPH" ) );
            REQUIRE( val["stages"][1]["stage"] == 2 );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][1]["lower"].as<double>() ), 0.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( ( float) ( val["stages"][1]["upper"].as<double>() ), 0.0F ) );
            REQUIRE( val["stages"][1]["minBlower"] == 0 );
            REQUIRE( val["stages"][1]["maxBlower"] == 0 );
            REQUIRE( val["stages"][1]["minOn"] == 0 );
            REQUIRE( val["stages"][1]["minOff"] == 0 );
            REQUIRE( STRCMP( val["stages"][1]["cph"], "e2CPH" ) );
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
            const char* json = "{name:\"APPLE\", val:{opMode:\"eHEATING\", iduType:\"eAIR_HANDLER\",oduType:\"eHP\",numCompStages:1,numIdStages:2,totalStages:2,stages:[{stage:1,lower:1,upper:10,minBlower:1,maxBlower:11},{stage:2,lower:2,upper:20,minBlower:3,maxBlower:4,minOn:11,minOff:22,cph:\"e5CPH\"}], fanCont:5, gain:11.5, reset:0.5, maxPv:100.5}}";
            bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON [%s]\nerrorMsg=[%s])", json, errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum2 == seqNum + 1 );
            Storm::Type::SystemConfig_T value;
            int8_t           valid = mp_apple_.read( value, &seqNum );
            REQUIRE( seqNum == seqNum2 );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp == &mp_apple_ );
            REQUIRE( value.currentOpMode == Storm::Type::OperatingMode::eHEATING );
            REQUIRE( value.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
            REQUIRE( value.outdoorUnitType == Storm::Type::OduType::eHP );
            REQUIRE( value.numCompressorStages == 1 );
            REQUIRE( value.numIndoorStages == 2 );
            REQUIRE( value.totalStages == 2 );
            REQUIRE( value.fanContinuousSpeed == 5 );
            REQUIRE( Cpl::Math::areFloatsEqual( value.gain, 11.5 ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.reset, 0.5 ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.maxPvOut, 100.5 ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].lowerBound, 1.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].upperBound, 10.0F) );
            REQUIRE( value.stages[0].minIndoorFan == 1 );
            REQUIRE( value.stages[0].maxIndoorFan == 11 );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].lowerBound, 2.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].upperBound, 20.0F ) );
            REQUIRE( value.stages[1].minIndoorFan == 3 );
            REQUIRE( value.stages[1].maxIndoorFan == 4 );
            REQUIRE( value.stages[1].minOnTime == 11 );
            REQUIRE( value.stages[1].minOffTime == 22 );
            REQUIRE( value.stages[1].cph == Storm::Type::Cph::e5CPH );
        }



        SECTION( "Write value - error cases" )
        {
            const char* json   = "{name:\"APPLE\", val:\"abc\"}";
            Storm::Type::SystemConfig_T expectedValue = { {{1.0F, 10.0F}, {2.0F, 20.0F}, }, Storm::Type::OperatingMode::eCOOLING, Storm::Type::OduType::eHP, Storm::Type::IduType::eAIR_HANDLER, 1, 2, 2 };
            uint16_t    seqnum = mp_apple_.write( expectedValue );
            bool        result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqnum == mp_apple_.getSequenceNumber() );  // No updated is Expected!

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
            json     = "{name:\"APPLE\", val:1}";
            seqnum   = mp_apple_.getSequenceNumber();
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqnum == mp_apple_.getSequenceNumber() );  // No updated is Expected!


            errorMsg = "noerror";
            json     = "{name:\"APPLE\", val:\"eoff\"}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqnum == mp_apple_.getSequenceNumber() );  // No updated is Expected!

            errorMsg = "noerror";
            json     = "{name:\"BOB\", invalid:1}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == false );
        }

        SECTION( "Set Invalid" )
        {
            Storm::Type::SystemConfig_T expectedValue = { {{1.0F, 10.0F}, {2.0F, 20.0F}, }, Storm::Type::OperatingMode::eCOOLING, Storm::Type::OduType::eHP, Storm::Type::IduType::eAIR_HANDLER, 1, 2, 2 };
            uint16_t seqNum = mp_apple_.write( expectedValue );
            const char* json = "{name:\"APPLE\", val:\"eAC1_AH0\", invalid:1}";
            bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum2 == seqNum + 1 );
            Storm::Type::SystemConfig_T value;
            int8_t                      valid = mp_apple_.read( value, &seqNum );
            REQUIRE( seqNum == seqNum2 );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == false );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp == &mp_apple_ );
        }

        SECTION( "lock..." )
        {
            const char* json = "{name:\"APPLE\", locked:true, val:{opMode:\"eCOOLING\", iduType:\"eAIR_HANDLER\",oduType:\"eHP\",numCompStages:1,numIdStages:2,totalStages:2,pvBounds:[{stage:1,lower:1,upper:10},{stage:2,lower:2,upper:20}]}}";
            bool result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            Storm::Type::SystemConfig_T value;
            int8_t valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp_apple_.isLocked() == true );
            REQUIRE( value.currentOpMode == Storm::Type::OperatingMode::eCOOLING );
            REQUIRE( value.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
            REQUIRE( value.outdoorUnitType == Storm::Type::OduType::eHP );
            REQUIRE( value.numCompressorStages == 1 );
            REQUIRE( value.numIndoorStages == 2 );
            REQUIRE( value.totalStages == 2 );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].lowerBound, 1.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].upperBound, 10.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].lowerBound, 2.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].upperBound, 20.0F ) );

            json   = "{name:\"APPLE\", invalid:21, locked:false}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isNotValid() == true );
            REQUIRE( mp_apple_.isLocked() == false );
            REQUIRE( mp_apple_.getValidState() == 21 );

            json = "{name:\"APPLE\", locked:true, val:{opMode:\"eCOOLING\", iduType:\"eAIR_HANDLER\",oduType:\"eAC\",numCompStages:1,numIdStages:2,totalStages:2,pvBounds:[{stage:1,lower:1,upper:10},{stage:2,lower:2,upper:20}]}}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isLocked() == true );
            valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( value.currentOpMode == Storm::Type::OperatingMode::eCOOLING );
            REQUIRE( value.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
            REQUIRE( value.outdoorUnitType == Storm::Type::OduType::eAC );
            REQUIRE( value.numCompressorStages == 1 );
            REQUIRE( value.numIndoorStages == 2 );
            REQUIRE( value.totalStages == 2 );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].lowerBound, 1.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].upperBound, 10.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].lowerBound, 2.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].upperBound, 20.0F ) );

            json = "{name:\"APPLE\", val:{opMode:\"eCOOLING\", iduType:\"eFURNACE\",oduType:\"eHP\",numCompStages:1,numIdStages:2,totalStages:2,pvBounds:[{stage:1,lower:1,upper:10},{stage:2,lower:2,upper:20}]}}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isLocked() == true );
            valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( value.currentOpMode == Storm::Type::OperatingMode::eCOOLING );
            REQUIRE( value.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
            REQUIRE( value.outdoorUnitType == Storm::Type::OduType::eAC );
            REQUIRE( value.numCompressorStages == 1 );
            REQUIRE( value.numIndoorStages == 2 );
            REQUIRE( value.totalStages == 2 );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].lowerBound, 1.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].upperBound, 10.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].lowerBound, 2.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].upperBound, 20.0F ) );

            json   = "{name:\"APPLE\", locked:false}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            valid = mp_apple_.read( value );
            REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
            REQUIRE( mp_apple_.isLocked() == false );
            REQUIRE( value.currentOpMode == Storm::Type::OperatingMode::eCOOLING );
            REQUIRE( value.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
            REQUIRE( value.outdoorUnitType == Storm::Type::OduType::eAC );
            REQUIRE( value.numCompressorStages == 1 );
            REQUIRE( value.numIndoorStages == 2 );
            REQUIRE( value.totalStages == 2 );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].lowerBound, 1.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[0].upperBound, 10.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].lowerBound, 2.0F ) );
            REQUIRE( Cpl::Math::areFloatsEqual( value.stages[1].upperBound, 20.0F ) );
        }
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

