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

#include "colony_config.h"
#include "Catch/catch.hpp"
#include "Driver/DIO/Pwm.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Uint32.h"


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::Dm::Mp::Uint32       mp_pwm_( modelDb_, "pwm" );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Pwm" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    uint32_t state;
    mp_pwm_.setInvalid();

    SECTION( "nominal" )
    {
        Driver::DIO::Pwm uut( mp_pwm_ );
        REQUIRE( mp_pwm_.isNotValid() );
        REQUIRE( uut.start( 5 ) );
        REQUIRE( mp_pwm_.read( state ) );
        REQUIRE( state == 5 );
        uut.setDutyCycle( 9 );
        REQUIRE( mp_pwm_.read( state ) );
        REQUIRE( state == 9 );
        uut.setDutyCycle( OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE +1 );
        REQUIRE( mp_pwm_.read( state ) );
        REQUIRE( state == OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE );
        uut.stop();
        REQUIRE( mp_pwm_.isNotValid() );
    }

    SECTION( "errors" )
    {
        Driver::DIO::Pwm uut( mp_pwm_ );
        mp_pwm_.write( 1 );  // Set MP valid
        uut.stop();
        REQUIRE( mp_pwm_.isNotValid() == false ); // If the stop() call worked - the MP would be invalid

        REQUIRE( uut.start( 10 ) );
        REQUIRE( mp_pwm_.read( state ) );
        REQUIRE( state == 10 );
        REQUIRE( uut.start( 100 ) == false );
        REQUIRE( mp_pwm_.read( state ) );
        REQUIRE( state == 10 );

        uut.stop();
        uut.setDutyCycle( 50 );
        REQUIRE( mp_pwm_.isNotValid() );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
