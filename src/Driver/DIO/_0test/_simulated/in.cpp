/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Driver/DIO/In.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Bool.h"


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::Dm::Mp::Bool         mp_in_( modelDb_, "in" );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "In" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    bool state;
    mp_in_.setInvalid();

    SECTION( "nominal" )
    {
        Driver::DIO::In uut( mp_in_ );
        REQUIRE( mp_in_.isNotValid() );
        REQUIRE( uut.start() );
        state = uut.sample();
        REQUIRE( state == false );
        mp_in_.write( true );
        state = uut.sample();
        REQUIRE( state == true );
        mp_in_.write( false );
        state = uut.sample();
        REQUIRE( state == false );
        uut.stop();
        REQUIRE( mp_in_.isNotValid() );
    }

    SECTION( "inverted" )
    {
        Driver::DIO::In uut( mp_in_, false );
        REQUIRE( mp_in_.isNotValid() );
        REQUIRE( uut.start() );
        state = uut.sample();
        REQUIRE( state == true );
        mp_in_.write( true );
        state = uut.sample();
        REQUIRE( state == false );
        mp_in_.write( false );
        state = uut.sample();
        REQUIRE( state == true );
        uut.stop();
        REQUIRE( mp_in_.isNotValid() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
