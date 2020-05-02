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
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "common.h"



////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_("ignoreThisParameter_usedToInvokeTheStaticConstructor");

// Allocate my Model Points
static StaticInfo       info_mp_apple_( "APPLE1" );
static Mp::Uint32       mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo       info_mp_orange_( "ORANGE1" );
static Mp::Uint32       mp_orange_( modelDb_, info_mp_orange_ );

static StaticInfo       info_mp_cherry_( "CHERRY1" );
static Mp::Uint32       mp_cherry_( modelDb_, info_mp_cherry_ );

static StaticInfo       info_mp_plum_( "PLUM1" );
static Mp::Uint32       mp_plum_( modelDb_, info_mp_plum_ );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "seqnum", "[seqnum]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "SEQNUM test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Test Sequence number
    uint16_t seqNum = mp_apple_.getSequenceNumber();
    REQUIRE( seqNum != ModelPoint::SEQUENCE_NUMBER_UNKNOWN );
    REQUIRE( mp_apple_.isNotValid() == true );

    uint16_t seqNum2 = mp_apple_.write( 10 );
    REQUIRE( seqNum + 1 == seqNum2 );

    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum2 + 1 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == true );

    seqNum2 = mp_apple_.write( 11 );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( seqNum + 1 == seqNum2 );

    seqNum = mp_apple_.write( 11 );
    REQUIRE( seqNum == seqNum2 );

    seqNum2 = mp_apple_.touch();
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( seqNum + 1 == seqNum2 );

    seqNum = mp_apple_.setInvalid();
    REQUIRE( seqNum2 + 1 == seqNum );
    REQUIRE( mp_apple_.isNotValid() == true );

    seqNum2 = mp_apple_.touch();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( seqNum + 1 == seqNum2 );

    // NOTE: Test must leave the model point in the invalid state

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "invalid", "[invalid]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "INVALID test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Invalid values
    int8_t validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( validState != ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( ModelPoint::IS_VALID( validState ) == false );

    mp_apple_.write( 0 );
    validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( validState == ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( ModelPoint::IS_VALID( validState ) );

    mp_apple_.setInvalidState( 100 );
    validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( validState != ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( validState == 100 );
    REQUIRE( ModelPoint::IS_VALID( validState ) == false );

    mp_apple_.setInvalidState( 101 );
    validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( validState != ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( validState == 101 );
    REQUIRE( ModelPoint::IS_VALID( validState ) == false );

    mp_apple_.setInvalidState( -1 );
    validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( validState != ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( validState == OPTION_CPL_DM_MODEL_POINT_STATE_INVALID );
    REQUIRE( ModelPoint::IS_VALID( validState ) == false );

    mp_apple_.write( 0 );
    validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( validState == ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( ModelPoint::IS_VALID( validState ) );

    mp_apple_.setInvalidState( -1 );
    validState = mp_apple_.getValidState();
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( validState != ModelPoint::MODEL_POINT_STATE_VALID );
    REQUIRE( validState == OPTION_CPL_DM_MODEL_POINT_STATE_INVALID );
    REQUIRE( ModelPoint::IS_VALID( validState ) == false );

    // NOTE: Test must leave the model point in the invalid state

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE( "lock", "[lock]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "LOCK test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    REQUIRE( mp_apple_.isNotValid() == true );
    bool locked = mp_apple_.isLocked();
    REQUIRE( locked == false );
    mp_apple_.applyLock();
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    mp_apple_.removeLock();
    locked = mp_apple_.isLocked();
    REQUIRE( locked == false );
    REQUIRE( mp_apple_.isNotValid() == true );

    mp_apple_.setInvalid( ModelPoint::eUNLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == false );

    mp_apple_.setInvalid( ModelPoint::eLOCK );
    REQUIRE( mp_apple_.isNotValid() == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );

    mp_apple_.write( 11 );
    uint32_t value;
    int8_t   valid;
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );

    mp_apple_.write( 12, ModelPoint::eUNLOCK );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == false );
    REQUIRE( value == 12 );

    RmwUint32 callbackClient;
    callbackClient.m_callbackCount  = 0;
    callbackClient.m_incValue       = 1;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == 12 + 1 );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    callbackClient.m_incValue       = 2;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( value == 12 + 1 );
    REQUIRE( callbackClient.m_callbackCount == 1 );

    callbackClient.m_incValue       = 3;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eUNLOCK );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == false );
    REQUIRE( value == 12 + 1 + 3 );
    REQUIRE( callbackClient.m_callbackCount == 2 );

    callbackClient.m_incValue       = 4;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eUNLOCK );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == false );
    REQUIRE( value == 12 + 1 + 3 + 4 );
    REQUIRE( callbackClient.m_callbackCount == 1 + 1 + 1 );

    callbackClient.m_incValue       = 5;
    callbackClient.m_returnResult   = ModelPoint::eINVALIDATE;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == true );
    REQUIRE( ModelPoint::IS_VALID( valid ) == false );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( callbackClient.m_callbackCount == 1 + 1 + 1 + 1 );

    callbackClient.m_incValue       = 5;
    callbackClient.m_returnResult   = ModelPoint::eCHANGED;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eLOCK );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( callbackClient.m_callbackCount == 1 + 1 + 1 + 1 + 1);

    callbackClient.m_incValue       = 5;
    callbackClient.m_returnResult   = ModelPoint::eINVALIDATE;
    mp_apple_.readModifyWrite( callbackClient );
    valid = mp_apple_.read( value);
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == true );
    REQUIRE( callbackClient.m_callbackCount == 1 + 1 + 1 + 1 + 1 );

    callbackClient.m_incValue       = 6;
    callbackClient.m_returnResult   = ModelPoint::eNO_CHANGE;
    mp_apple_.readModifyWrite( callbackClient, ModelPoint::eUNLOCK );
    valid = mp_apple_.read( value);
	REQUIRE( value == 30 );
    REQUIRE( mp_apple_.isNotValid() == false );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    locked = mp_apple_.isLocked();
    REQUIRE( locked == false );
    REQUIRE( callbackClient.m_callbackCount == 1 + 1 + 1 + 1 + 1 + 1 );

    // NOTE: Test must leave the model point in the invalid state
	mp_apple_.setInvalid();

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "find", "[find]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "FIND test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Look up non existent MPs
    Mp::Uint32* mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "APPLE" );
    REQUIRE( mp == 0 );
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "ORANGE" );
    REQUIRE( mp == 0 );
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "CHERRY" );
    REQUIRE( mp == 0 );
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "PLUM" );
    REQUIRE( mp == 0 );

    // Look up MPs
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "APPLE1" );
    REQUIRE( mp == &mp_apple_ );
    REQUIRE( mp->isNotValid() == true );
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "ORANGE1" );
    REQUIRE( mp ==&mp_orange_);
    REQUIRE( mp->isNotValid() == true );
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "CHERRY1" );
    REQUIRE( mp == &mp_cherry_ );
    REQUIRE( mp->isNotValid() == true );
    mp = ( Mp::Uint32*) modelDb_.lookupModelPoint( "PLUM1" );
    REQUIRE( mp == &mp_plum_ );
    REQUIRE( mp->isNotValid() == true );
    
    // Operated on found MP
    mp->write( 10 );
    REQUIRE( mp->isNotValid() == false );
    REQUIRE( mp_plum_.isNotValid() == false );
    uint32_t value;
    int8_t   valid = mp_plum_.read( value );
    REQUIRE( ModelPoint::IS_VALID( valid ) == true );
    REQUIRE( value == 10 );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "runtime_db", "[runtime_db]" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "RUNTIME_DB test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create a Model Database and Model Points at run time
    ModelDatabase myDb;
    Mp::Uint32    myApple( myDb, info_mp_apple_ );
    Mp::Uint32    myCherry( myDb, info_mp_cherry_ );
        

    // Look up MPs
    Mp::Uint32* mp = ( Mp::Uint32*) myDb.lookupModelPoint( "APPLE1" );
    REQUIRE( mp == &myApple );
    REQUIRE( mp->isNotValid() == true );
    mp = ( Mp::Uint32*) myDb.lookupModelPoint( "ORANGE1" );
    REQUIRE( mp == 0 );
    mp = ( Mp::Uint32*) myDb.lookupModelPoint( "CHERRY1" );
    REQUIRE( mp->isNotValid() == true );
    REQUIRE( mp == &myCherry );
    mp = ( Mp::Uint32*) myDb.lookupModelPoint( "PLUM1" );
    REQUIRE( mp == 0 );

    // Traverse the Database
    ModelPoint* mpPtr = myDb.getFirstByName();
    REQUIRE( strcmp( mpPtr->getName(), "APPLE1") == 0 );
    mpPtr = myDb.getNextByName( *mpPtr );
    REQUIRE( strcmp( mpPtr->getName(), "CHERRY1") == 0 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}