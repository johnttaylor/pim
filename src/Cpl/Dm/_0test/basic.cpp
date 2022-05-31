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
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Uint32       mp_apple_( modelDb_, "APPLE1" );
static Mp::Uint32       mp_orange_( modelDb_, "ORANGE1" );
static Mp::Uint32       mp_cherry_( modelDb_, "CHERRY1" );
static Mp::Uint32       mp_plum_( modelDb_, "PLUM1" );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "common" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "seqnum" )
    {
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
    }

    SECTION( "invalid" )
    {
        // Invalid values
        REQUIRE( mp_apple_.isNotValid() == true );

        mp_apple_.write( 0 );
        REQUIRE( mp_apple_.isNotValid() == false );

        mp_apple_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == true );

        mp_apple_.write( 0 );
        REQUIRE( mp_apple_.isNotValid() == false );

        mp_apple_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == true );

        // NOTE: Test must leave the model point in the invalid state
    }

    ///////////////////////////////////////////////////////////////////////////////
    SECTION( "lock" )
    {
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
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( valid == false );
        locked = mp_apple_.isLocked();
        REQUIRE( locked == true );

        mp_apple_.write( 12, ModelPoint::eUNLOCK );
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        locked = mp_apple_.isLocked();
        REQUIRE( locked == false );
        REQUIRE( value == 12 );


        // NOTE: Test must leave the model point in the invalid state
        mp_apple_.setInvalid();
    }

    SECTION( "find" )
    {
        // Look up non existent MPs
        Mp::Uint32* mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "APPLE" );
        REQUIRE( mp == 0 );
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "ORANGE" );
        REQUIRE( mp == 0 );
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "CHERRY" );
        REQUIRE( mp == 0 );
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "PLUM" );
        REQUIRE( mp == 0 );

        // Look up MPs
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "APPLE1" );
        REQUIRE( mp == &mp_apple_ );
        REQUIRE( mp->isNotValid() == true );
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "ORANGE1" );
        REQUIRE( mp == &mp_orange_ );
        REQUIRE( mp->isNotValid() == true );
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "CHERRY1" );
        REQUIRE( mp == &mp_cherry_ );
        REQUIRE( mp->isNotValid() == true );
        mp = (Mp::Uint32*) modelDb_.lookupModelPoint( "PLUM1" );
        REQUIRE( mp == &mp_plum_ );
        REQUIRE( mp->isNotValid() == true );

        // Operated on found MP
        mp->write( 10 );
        REQUIRE( mp->isNotValid() == false );
        REQUIRE( mp_plum_.isNotValid() == false );
        uint32_t value;
        bool    valid = mp_plum_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 10 );
    }

    SECTION( "runtime_db" )
    {
        // Create a Model Database and Model Points at run time
        ModelDatabase myDb;
        Mp::Uint32    myApple( myDb, "APPLE1" );
        Mp::Uint32    myCherry( myDb, "CHERRY1" );


        // Look up MPs
        Mp::Uint32* mp = (Mp::Uint32*) myDb.lookupModelPoint( "APPLE1" );
        REQUIRE( mp == &myApple );
        REQUIRE( mp->isNotValid() == true );
        mp = (Mp::Uint32*) myDb.lookupModelPoint( "ORANGE1" );
        REQUIRE( mp == 0 );
        mp = (Mp::Uint32*) myDb.lookupModelPoint( "CHERRY1" );
        REQUIRE( mp->isNotValid() == true );
        REQUIRE( mp == &myCherry );
        mp = (Mp::Uint32*) myDb.lookupModelPoint( "PLUM1" );
        REQUIRE( mp == 0 );

        // Traverse the Database
        ModelPoint* mpPtr = myDb.getFirstByName();
        REQUIRE( strcmp( mpPtr->getName(), "APPLE1" ) == 0 );
        mpPtr = myDb.getNextByName( *mpPtr );
        REQUIRE( strcmp( mpPtr->getName(), "CHERRY1" ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}