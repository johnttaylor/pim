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
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Driver/Button/PolledDebounced.h"

using namespace Driver::Button;

///////////////////////////////////////////////////////////////////////////////
/// Mock the HAL method(s)

#define NUM_BUTTONS     3

#define APPLE_HDL       0
#define ORANGE_HDL      1
#define CHERRY_HDL      2

static bool mockedButtonsPressed_[NUM_BUTTONS]{ false, false, false };
bool driverButtonHalUnitTest_getRawPressState( int pinHandle )
{
    return mockedButtonsPressed_[pinHandle];
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PolledDebounced" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    PolledDebounced apple( APPLE_HDL );
    PolledDebounced orange( ORANGE_HDL, 3 );
    PolledDebounced cherry( CHERRY_HDL, 4 );

    SECTION( "handles" )
    {
        REQUIRE( apple.getHandle() == APPLE_HDL );
        REQUIRE( orange.getHandle() == ORANGE_HDL );
        REQUIRE( cherry.getHandle() == CHERRY_HDL );
    }

    SECTION( "happy path" )
    {
        apple.start();
        orange.start();
        cherry.start();

        // Initial transitions
        mockedButtonsPressed_[APPLE_HDL]  = true;
        mockedButtonsPressed_[ORANGE_HDL] = true;
        mockedButtonsPressed_[CHERRY_HDL] = true;
        apple.sample();
        orange.sample();
        cherry.sample();

        REQUIRE( apple.isPressed() == false );
        REQUIRE( orange.isPressed() == false );
        REQUIRE( cherry.isPressed() == false );

        // Count 1
        apple.sample();
        orange.sample();
        cherry.sample();

        REQUIRE( apple.isPressed() == false );
        REQUIRE( orange.isPressed() == false );
        REQUIRE( cherry.isPressed() == false );

        // Count 2
        apple.sample();
        orange.sample();
        cherry.sample();

        REQUIRE( apple.isPressed() == true );
        REQUIRE( orange.isPressed() == false );
        REQUIRE( cherry.isPressed() == false );

        // Count 3
        apple.sample();
        orange.sample();
        cherry.sample();

        REQUIRE( apple.isPressed() == true );
        REQUIRE( orange.isPressed() == true );
        REQUIRE( cherry.isPressed() == false );

        // Count 4
        apple.sample();
        orange.sample();
        cherry.sample();

        REQUIRE( apple.isPressed() == true );
        REQUIRE( orange.isPressed() == true );
        REQUIRE( cherry.isPressed() == true );
    }

    SECTION( "bouncing" )
    {
        cherry.start();

        // Initial transition
        mockedButtonsPressed_[CHERRY_HDL] = true;
        cherry.sample();
        REQUIRE( cherry.isPressed() == false );

        // Counts 1- 3
        cherry.sample();
        cherry.sample();
        cherry.sample();
        REQUIRE( cherry.isPressed() == false );

        // Release
        mockedButtonsPressed_[CHERRY_HDL] = false;
        cherry.sample();
        REQUIRE( cherry.isPressed() == false );

        // Single count
        mockedButtonsPressed_[CHERRY_HDL] = true;
        cherry.sample();
        REQUIRE( cherry.isPressed() == false );

        // Counts 1- 3
        cherry.sample();
        cherry.sample();
        cherry.sample();
        REQUIRE( cherry.isPressed() == false );

        // Count 4
        cherry.sample();
        REQUIRE( cherry.isPressed() == true );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}