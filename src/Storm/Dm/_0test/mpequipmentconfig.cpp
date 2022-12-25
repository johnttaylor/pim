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
        expectedVal ={ Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 1, true };
        Viewer<MpEquipmentConfig, Storm::Dm::MpEquipmentConfig::Data>    viewer_apple1( t1Mbox, Cpl::System::Thread::getCurrent(), mp_apple_ , expectedVal);
        Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Created Viewer thread (%p)", t1) );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        mp_apple_.setInvalid();
        viewer_apple1.open();
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

