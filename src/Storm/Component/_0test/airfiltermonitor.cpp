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
#include "Storm/Component/AirFilterMonitor.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"


#define FAN_CONT_SPEED  499

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "AirFilterMonitor" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    AirFilterMonitor::Input_T  ins  ={ &mp_maxAirFilterHours,  &mp_airFilterOperationTime, &mp_vOutputs, &mp_airFilterAlert };
    AirFilterMonitor::Output_T outs ={ &mp_airFilterAlert, &mp_airFilterOperationTime };

    // Initialize my inputs
    mp_airFilterAlert.write( { false, false, false } );
    mp_maxAirFilterHours.write( 10 );
    mp_airFilterOperationTime.write( { 0,0 } );

    AirFilterMonitor uut( ins, outs );

    // Start the component
    Cpl::System::ElapsedTime::Precision_T time ={ 1, 0 };
    uut.start( time );

    SECTION( "fan off" )
    {
        Cpl::System::ElapsedTime::Precision_T value;
        Cpl::System::ElapsedTime::Precision_T expected;
        mp_airFilterOperationTime.read( value );
        REQUIRE( value == expected );

        time += { 1, 0 };
        uut.doWork( true, time );
        time += { 1 * 60 * 60, 0 };
        uut.doWork( true, time );

        mp_airFilterOperationTime.read( value );
        REQUIRE( value == expected );
    }

    SECTION( "fan on" )
    {
        Cpl::System::ElapsedTime::Precision_T value;
        Cpl::System::ElapsedTime::Precision_T expected;
        mp_airFilterOperationTime.read( value );
        REQUIRE( value == expected );
        mp_vOutputs.setIndoorFanOutput( STORM_DM_MP_VIRTUAL_OUTPUTS_ON );

        time += { 1, 0 };
        uut.doWork( true, time );
        time += { 1 * 60 * 60, 0 };
        uut.doWork( true, time );

        mp_airFilterOperationTime.read( value );
        expected ={ 1 * 60 * 60, 0 };
        REQUIRE( value == expected );
    }

    SECTION( "alert" )
    {
        Cpl::System::ElapsedTime::Precision_T value;
        Cpl::System::ElapsedTime::Precision_T expected;
        mp_maxAirFilterHours.write( 2 );
        mp_airFilterOperationTime.read( value );
        REQUIRE( value == expected );
        mp_vOutputs.setIndoorFanOutput( STORM_DM_MP_VIRTUAL_OUTPUTS_ON );

        time += { 1, 0 };
        uut.doWork( true, time );
        time += { 1 * 60 * 60 / 2, 0 };
        time += { 1 * 60 * 60 / 2, 0 };
        uut.doWork( true, time );

        mp_airFilterOperationTime.read( value );
        expected ={ 1 * 60 * 60, 0 };
        REQUIRE( value == expected );
        Storm::Dm::MpSimpleAlarm::Data alert;
        mp_airFilterAlert.read( alert );
        REQUIRE( alert.acked == false );
        REQUIRE( alert.active == false );
        REQUIRE( alert.critical == false );

        time += { 1 * 60 * 60 / 2, 0 };
        time += { 1 * 60 * 60 / 2, 0 };
        uut.doWork( true, time );

        mp_airFilterOperationTime.read( value );
        expected ={ 0, 0 };
        REQUIRE( value == expected );
        mp_airFilterAlert.read( alert );
        REQUIRE( alert.acked == false );
        REQUIRE( alert.active == true );
        REQUIRE( alert.critical == false );

        time += { 1 * 60 * 60 / 2, 0 };
        time += { 1 * 60 * 60 / 2, 0 };
        uut.doWork( true, time );
      
        mp_airFilterOperationTime.read( value );
        REQUIRE( value == expected );
        mp_airFilterAlert.read( alert );
        REQUIRE( alert.acked == false );
        REQUIRE( alert.active == true );
        REQUIRE( alert.critical == false );

        mp_airFilterAlert.acknowledgeAlarm();
        time += { 1 * 60 * 60 / 2, 0 };
        time += { 1 * 60 * 60 / 2, 0 };
        uut.doWork( true, time );
       
        mp_airFilterOperationTime.read( value );
        REQUIRE( value == expected );
        mp_airFilterAlert.read( alert );
        REQUIRE( alert.acked == true );
        REQUIRE( alert.active == true );
        REQUIRE( alert.critical == false );

        mp_airFilterAlert.setAlarm( false );
        time += { 1, 0 };
        uut.doWork( true, time );
        time += { 1 * 60 * 60 / 2, 0 };
        uut.doWork( true, time );
      
        mp_airFilterOperationTime.read( value );
        expected ={ 1 * 60 * 60 / 2 , 0 };
        printf( "value=%lu.%u, expected=%lu.%u\n", (unsigned long)value.m_seconds, value.m_thousandths, (unsigned long)expected.m_seconds, expected.m_thousandths );
        REQUIRE( value == expected );
        mp_airFilterAlert.read( alert );
        REQUIRE( alert.acked == false );
        REQUIRE( alert.active == false );
        REQUIRE( alert.critical == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

