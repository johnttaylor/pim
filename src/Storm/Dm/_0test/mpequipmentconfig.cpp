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
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/DString.h"
#include "Cpl/Math/real.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Storm/Dm/MpEquipmentConfig.h"
#include "Storm/Constants.h"
#include "common.h"
#include <string.h>


using namespace Storm::Dm;

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"


////////////////////////////////////////////////////////////////////////////////
// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MpEquipmentConfig          mp_apple_( modelDb_, "APPLE" );
static MpEquipmentConfig          mp_orange_( modelDb_, "ORANGE", Storm::Type::IduType::eAIR_HANDLER, true, 10, Storm::Type::OduType::eAC, 0 );



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpEquipmentConfig" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    Storm::Dm::MpEquipmentConfig::Data value;
    Storm::Dm::MpEquipmentConfig::Data expectedVal;

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
        REQUIRE( strcmp( mpType, "Storm::Dm::MpEquipmentConfig" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        // Read
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.numCompStages == 0 );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );

        // Read
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == 1 );
        REQUIRE( value.iduType == Storm::Type::IduType::eFURNACE );
        REQUIRE( value.numCompStages == 1 );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );

        // Write
        expectedVal ={ Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 0, true };
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        // Write out-of-range
        expectedVal ={ Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eHP, 10, 10, true };
        mp_apple_.write( expectedVal );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.oduType == Storm::Type::OduType::eHP );
        REQUIRE( value.numCompStages == 0 );

        // Single writes
        mp_apple_.writeIndoorType( Storm::Type::IduType::eAIR_HANDLER );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );

        mp_apple_.writeIndoorFanMotor( false );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );

        // Single writes
        mp_apple_.writeIndoorHeatingStages( 0 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );

        // Single writes (out-of-range
        mp_apple_.writeIndoorType( Storm::Type::IduType::eFURNACE );
        mp_apple_.writeIndoorHeatingStages( 30 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == STORM_MAX_INDOOR_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eFURNACE );

        // Single writes
        mp_apple_.writeOutdoorType( Storm::Type::OduType::eHP );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.numCompStages == 0 );
        REQUIRE( value.oduType == Storm::Type::OduType::eHP );

        // Single writes
        mp_apple_.writeCompressorStages( 0 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.numCompStages == 0 );
        REQUIRE( value.oduType == Storm::Type::OduType::eHP );

        // Single writes (out-of-range
        mp_apple_.writeOutdoorType( Storm::Type::OduType::eAC );
        mp_apple_.writeCompressorStages( 3 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value.numCompStages == 1 );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );
    }

    SECTION( "copy" )
    {
        expectedVal ={ Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 0, true };
        mp_apple_.write( expectedVal );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        mp_orange_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == false );
        mp_apple_.copyFrom( mp_orange_ );
        REQUIRE( mp_apple_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        expectedVal ={ Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 0, true };
        mp_apple_.write( expectedVal );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("toJSON: [%s]", string) );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"]["oduType"], "eAC" ) );
        REQUIRE( STRCMP( doc["val"]["iduType"], "eAIR_HANDLER") );
        REQUIRE( doc["val"]["vspBlower"] == true );
        REQUIRE( doc["val"]["numCompStages"] == 1 );
        REQUIRE( doc["val"]["numIduHeat"] == 0  );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:{oduType:\"eAC\",iduType:\"eFURNACE\",vspBlower:false,numCompStages:0,numIduHeat:1}}";
        bool result = modelDb_.fromJSON( json, &errorMsg );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("errorMsg: [%s]", errorMsg.getString()) );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        expectedVal ={ Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, false };
        REQUIRE( memcmp( &value, &expectedVal, sizeof( value ) ) == 0 );

        json = "{name:\"APPLE\", val:{beginIndoorOffTimeSec:abc}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        Cpl::Dm::MailboxServer       t1Mbox;
        Viewer<MpEquipmentConfig>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ );
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        mp_apple_.setInvalid();
        viewer_apple1.open();
        expectedVal ={ Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, true };
        mp_apple_.write( expectedVal );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Waiting for viewer signal...") );
        Cpl::System::Thread::wait();
        viewer_apple1.close();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer closed.") );

        // Shutdown threads
        t1Mbox.pleaseStop();
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Destroying Viewer thread (%p)...", t1) );
        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Api::sleep( 100 ); // allow time for threads to stop BEFORE the runnable object goes out of scope
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

#if 0
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
#include "Cpl/Math/real.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Storm/Dm/MpEquipmentConfig.h"
#include "Storm/Constants.h"
#include "common.h"
#include <string.h>



using namespace Storm::Dm;
static Cpl::Dm::MailboxServer     t1Mbox_;

#define STRCMP(a,b)               (strcmp(a,b) == 0)

/////////////////////////////////////////////////////////////////
namespace {
class Rmw : public MpEquipmentConfig::Client
{
public:
    ///
    int                                         m_callbackCount;
    ///
    Cpl::Dm::ModelPoint::RmwCallbackResult_T    m_returnResult;
    ///
    uint16_t                                    m_newStages;

public:
    ///
    Rmw() :m_callbackCount( 0 ), m_returnResult( Cpl::Dm::ModelPoint::eNO_CHANGE ), m_newStages( 0 ) {}
    ///
    Cpl::Dm::ModelPoint::RmwCallbackResult_T callback( MpEquipmentConfig::Data& data, int8_t validState ) noexcept
    {
        m_callbackCount++;
        if ( m_returnResult != Cpl::Dm::ModelPoint::eNO_CHANGE )
        {
            data.numIduHeatingStages= m_newStages;
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
static MpEquipmentConfig        mp_apple_( modelDb_, info_mp_apple_ );

static Cpl::Dm::StaticInfo      info_mp_orange_( "ORANGE" );
static MpEquipmentConfig        mp_orange_( modelDb_, info_mp_orange_, Storm::Type::IduType::eAIR_HANDLER, true, 10, Storm::Type::OduType::eAC, 0 );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MP Equipment Unit Config" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "ReadWrite" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "READWRITE test" );

        // Read
        MpEquipmentConfig::Data value;
        uint16_t          seqNum;
        int8_t            valid = mp_orange_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.numCompStages == 0 );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );
        valid = mp_apple_.read( value, &seqNum );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == 1 );
        REQUIRE( value.iduType == Storm::Type::IduType::eFURNACE );
        REQUIRE( value.numCompStages == 1 );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );

        // Write
        value = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 0, true };
        uint16_t seqNum2 = mp_apple_.write( value );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );
        REQUIRE( value.numCompStages == 1 );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Write out-of-range
        value = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eHP, 10, 10, true };
        seqNum = mp_apple_.write( value );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.oduType == Storm::Type::OduType::eHP );
        REQUIRE( value.numCompStages == 0 );

        // Read-Modify-Write with Lock
        Rmw callbackClient;
        callbackClient.m_callbackCount  = 0;
        callbackClient.m_newStages      = 0;
        callbackClient.m_returnResult   = Cpl::Dm::ModelPoint::eCHANGED;
        mp_apple_.readModifyWrite( callbackClient, Cpl::Dm::ModelPoint::eLOCK );
        valid         = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        bool locked = mp_apple_.isLocked();
        REQUIRE( locked == true );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( callbackClient.m_callbackCount == 1 );

        // Read-Modify-Write with out-of-range values
        callbackClient.m_callbackCount  = 0;
        callbackClient.m_newStages      = 2;
        callbackClient.m_returnResult   = Cpl::Dm::ModelPoint::eCHANGED;
        mp_apple_.readModifyWrite( callbackClient, Cpl::Dm::ModelPoint::eUNLOCK );
        valid         = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( callbackClient.m_callbackCount == 1 );

        // Invalidate with Unlock
        mp_apple_.setInvalidState( 112, Cpl::Dm::ModelPoint::eUNLOCK );
        REQUIRE( mp_apple_.isNotValid() == true );
        valid = mp_apple_.getValidState();
        REQUIRE( valid == false );
        REQUIRE( valid == 112 );

        // Single writes
        mp_apple_.writeIndoorType( Storm::Type::IduType::eAIR_HANDLER );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );

        mp_apple_.writeIndoorFanMotor( false );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );

        // Single writes
        mp_apple_.writeIndoorHeatingStages( 0 );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );

        // Single writes (out-of-range
        mp_apple_.writeIndoorType( Storm::Type::IduType::eFURNACE );
        mp_apple_.writeIndoorHeatingStages( 30 );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == STORM_MAX_INDOOR_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eFURNACE );

        // Single writes
        mp_apple_.writeOutdoorType( Storm::Type::OduType::eHP );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.numCompStages == 0 );
        REQUIRE( value.oduType == Storm::Type::OduType::eHP );

        // Single writes
        mp_apple_.writeCompressorStages( 0 );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.numCompStages == 0 );
        REQUIRE( value.oduType == Storm::Type::OduType::eHP );

        // Single writes (out-of-range
        mp_apple_.writeOutdoorType( Storm::Type::OduType::eAC );
        mp_apple_.writeCompressorStages( 3 );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.numCompStages == 1 );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );
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
        REQUIRE( s == sizeof( MpEquipmentConfig::Data ) );
        s = mp_orange_.getSize();
        REQUIRE( s == sizeof( MpEquipmentConfig::Data ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( MpEquipmentConfig::Data ) + sizeof( int8_t ) );
        s = mp_orange_.getExternalSize();
        REQUIRE( s == sizeof( MpEquipmentConfig::Data ) + sizeof( int8_t ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s]", mpType ) );
        REQUIRE( strcmp( mpType, "Storm::Dm::MpEquipmentConfig" ) == 0 );
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
        MpEquipmentConfig::Data value = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 0, true };
        seqNum = mp_apple_.write( value );
        REQUIRE( seqNum == seqNum2 + 1 );
        int8_t valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( value.hasVspBlower == true );
        REQUIRE( value.numIduHeatingStages == 0 );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );
        REQUIRE( value.numCompStages == 1 );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( valid == false );

        // Update the MP
        value = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 1, false };
        seqNum = mp_apple_.write( value );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
        REQUIRE( value.oduType == Storm::Type::OduType::eAC );
        REQUIRE( value.numCompStages == 1 );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == false );
        b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // Invalidate the MP
        seqNum = mp_apple_.setInvalid();
        REQUIRE( seqNum == seqNum2 + 1 );
        REQUIRE( mp_apple_.isNotValid() == true );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value.hasVspBlower == false );
        REQUIRE( value.numIduHeatingStages == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
        REQUIRE( value.iduType == Storm::Type::IduType::eAIR_HANDLER );
    }

    ///////////////////////////////////////////////////////////////////////////////


    SECTION( "observer" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "observer TEST" );

        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
        AsyncClient<MpEquipmentConfig> viewer1( t1Mbox_, Cpl::System::Thread::getCurrent(), mp_apple_ );

        // Open, write a value, wait for Viewer to see the change, then close
        mp_apple_.removeLock();
        viewer1.open();
        MpEquipmentConfig::Data value = { Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 1, 1, true };
        uint16_t seqNum = mp_apple_.write( value );
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
            MpEquipmentConfig::Data value = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 1, false };
            uint16_t seqnum  = mp_apple_.write( value, Cpl::Dm::ModelPoint::eUNLOCK );
            mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "toJSON: [%s])", string ) );

            StaticJsonDocument<1024> doc;
            DeserializationError err = deserializeJson( doc, string );
            REQUIRE( err == DeserializationError::Ok );
            REQUIRE( doc["seqnum"] == seqnum );
            REQUIRE( doc["locked"] == false );
            REQUIRE( doc["invalid"] == 0 );
            JsonObject val = doc["val"];
            REQUIRE( STRCMP( val["iduType"], "eAIR_HANDLER" ) );
            REQUIRE( val["vspBlower"] == false );
            REQUIRE( val["numIduHeat"] == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
            REQUIRE( STRCMP( val["oduType"], "eAC" ) );
            REQUIRE( val["numCompStages"] == 1 );
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
            JsonObject val = doc["val"];
            REQUIRE( STRCMP( val["iduType"], "eAIR_HANDLER" ) );
            REQUIRE( val["vspBlower"] == false );
            REQUIRE( val["numIduHeat"] == OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES );
            REQUIRE( STRCMP( val["oduType"], "eAC" ) );
            REQUIRE( val["numCompStages"] == 1 );
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
            const char* json = "{name:\"APPLE\", val:{iduType:\"eFURNACE\", vspBlower:true, numIduHeat:10}}";
            bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum2 == seqNum + 1 );
            MpEquipmentConfig::Data value;
            int8_t valid;
            valid = mp_apple_.read( value, &seqNum );
            REQUIRE( seqNum == seqNum2 );
            REQUIRE( valid );
            REQUIRE( value.hasVspBlower == true );
            REQUIRE( value.numIduHeatingStages == STORM_MAX_INDOOR_STAGES );
            REQUIRE( value.iduType == Storm::Type::IduType::eFURNACE );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp == &mp_apple_ );
        }

        SECTION( "Write value - error cases" )
        {
            const char* json   = "{name:\"APPLE\", val:\"abc\"}";
            uint16_t    seqnum = mp_apple_.writeIndoorType( Storm::Type::IduType::eFURNACE );
            bool        result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqnum == mp_apple_.getSequenceNumber() ); // Expect result is NO update

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
            json     = "{name:\"APPLE\", val:a123}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == false );
            REQUIRE( errorMsg != "noerror" );

            errorMsg = "noerror";
            json     = "{name:\"APPLE\", val:{}}";
            seqnum   = mp_apple_.getSequenceNumber();
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqnum == mp_apple_.getSequenceNumber() ); // Expect result is NO update

            errorMsg = "noerror";
            json     = "{name:\"APPLE\", val:{numIduHeat:a}}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg =[%s])", errorMsg.getString() ) );
            REQUIRE( result == false );
            REQUIRE( errorMsg != "noerror" );

            errorMsg = "noerror";
            json     = "{name:\"BOB\", invalid:1}";
            result   = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == false );
        }


        SECTION( "Set Invalid" )
        {
            MpEquipmentConfig::Data value = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 1, true };
            uint16_t seqNum = mp_apple_.write( value );
            const char* json = "{name:\"APPLE\", val:{vspBlower:true}, invalid:1}";
            bool result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( seqNum2 == seqNum + 1 );
            int8_t valid;
            valid = mp_apple_.read( value, &seqNum );
            REQUIRE( seqNum == seqNum2 );
            REQUIRE( valid == false );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp == &mp_apple_ );
        }

        SECTION( "lock..." )
        {
            const char* json = "{name:\"APPLE\", val:{vspBlower:true}, locked:true}";
            bool result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            int8_t valid;
            MpEquipmentConfig::Data value;
            valid = mp_apple_.read( value );
            REQUIRE( valid == true );
            REQUIRE( errorMsg == "noerror" );
            REQUIRE( mp_apple_.isLocked() == true );
            REQUIRE( value.hasVspBlower == true );
            REQUIRE( value.numIduHeatingStages == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES );
            REQUIRE( value.iduType == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE );

            json   = "{name:\"APPLE\", invalid:21, locked:false}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isNotValid() == true );
            REQUIRE( mp_apple_.isLocked() == false );
            REQUIRE( mp_apple_.getValidState() == 21 );

            json   = "{name:\"APPLE\", val:{numIduHeat:0}, locked:true}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            REQUIRE( mp_apple_.isLocked() == true );
            valid = mp_apple_.read( value );
            REQUIRE( valid == true );
            REQUIRE( value.hasVspBlower == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER );
            REQUIRE( value.numIduHeatingStages == 0 );
            REQUIRE( value.iduType == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE );

            json   = "{name:\"APPLE\", locked:false}";
            result = modelDb_.fromJSON( json, &errorMsg );
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "fromSJON errorMsg=[%s])", errorMsg.getString() ) );
            REQUIRE( result == true );
            valid = mp_apple_.read( value );
            REQUIRE( valid == true );
            REQUIRE( value.hasVspBlower == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER );
            REQUIRE( value.numIduHeatingStages == 0 );
            REQUIRE( value.iduType == OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE );
            REQUIRE( mp_apple_.isLocked() == false );
        }
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

#endif