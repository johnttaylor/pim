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
#include "Storm/Component/HvacRelayOutputs.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"
#include <string.h>


using namespace Storm::Component;

#define SECT_       "_0test"


#define VIRTUAL_FAN_SPEED   500
#define FAN_OUT_SPEED       (VIRTUAL_FAN_SPEED/10)

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "HvacRelayOutputs" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    HvacRelayOutputs::Input_T  ins  = { &mp_vOutputs, &mp_equipmentBeginTimes, &mp_systemForcedOffRefCnt, &mp_systemOn };
    HvacRelayOutputs::Output_T outs = { &mp_equipmentBeginTimes, &mp_relayOutputs };

    Storm::Type::EquipmentTimes_T    equipTimes;
    Storm::Type::HvacRelayOutputs_T  relays;
    Storm::Type::HvacRelayOutputs_T  val;
    memset( &relays, 0, sizeof( relays ) );
    memset( &val, 0, sizeof( val ) );
    mp_vOutputs.setInvalid();       // Clear the MP
    mp_vOutputs.setSafeAllOff();    // Make the MP valid (with its default values)
    mp_systemForcedOffRefCnt.reset();
    mp_systemOn.write( false );

    HvacRelayOutputs uut( ins, outs );

    // Start the components
    Cpl::System::ElapsedTime::Precision_T time = { 1, 0 };
    uut.start( time );

    // Everything off
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = false; val.w2 = false, val.w3 = false; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == time );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == time );
    REQUIRE( equipTimes.systemBeginOffTime == time );
    Cpl::System::ElapsedTime::Precision_T prevTime = time;

    // Turn on the Indoor/Outdoor door units
    mp_systemOn.write( true );
    mp_vOutputs.setIndoorStageOutput( 0, STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
    mp_vOutputs.setOutdoorStageOutput( 0, STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = true; val.w2 = false, val.w3 = false; val.y1 = true, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == time );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == time );
    REQUIRE( equipTimes.systemBeginOnTime == time );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == prevTime );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == prevTime );
    REQUIRE( equipTimes.systemBeginOffTime == prevTime );
    Cpl::System::ElapsedTime::Precision_T prevTime2 = time;

    // Turn outdoor off
    mp_vOutputs.setOutdoorStageOutput( 0, STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = true; val.w2 = false, val.w3 = false; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.systemBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == prevTime );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == time );
    REQUIRE( equipTimes.systemBeginOffTime == prevTime );
    Cpl::System::ElapsedTime::Precision_T prevTime3 = time;

    // Forced off
    mp_systemForcedOffRefCnt.increment();
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = false; val.w2 = false, val.w3 = false; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.systemBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == time );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == prevTime3 );
    REQUIRE( equipTimes.systemBeginOffTime == prevTime );
    Cpl::System::ElapsedTime::Precision_T prevTime4 = time;

    // Turn indoor off
    mp_systemForcedOffRefCnt.reset();
    mp_vOutputs.setIndoorStageOutput( 0, STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = false; val.w2 = false, val.w3 = false; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.systemBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == prevTime4 );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == prevTime3 );
    REQUIRE( equipTimes.systemBeginOffTime == prevTime );
    //Cpl::System::ElapsedTime::Precision_T prevTime5 = time;

    // Turn system off 
    mp_systemOn.write( false );
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = false; val.w2 = false, val.w3 = false; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.systemBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == prevTime4 );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == prevTime3 );
    REQUIRE( equipTimes.systemBeginOffTime == time );
    Cpl::System::ElapsedTime::Precision_T prevTime6 = time;

    // Turn on remaining indoor outputs
    mp_vOutputs.setIndoorStageOutput( 1, STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
    mp_vOutputs.setIndoorStageOutput( 2, STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
    mp_vOutputs.setIndoorFanOutput( VIRTUAL_FAN_SPEED );
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = FAN_OUT_SPEED; val.g = true; val.o = true; val.w1 = false; val.w2 = true, val.w3 = true; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.systemBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == prevTime4 );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == prevTime3 );
    REQUIRE( equipTimes.systemBeginOffTime == prevTime6 );

    // Invalid input
    mp_systemOn.setInvalid();
    time.m_seconds += 1;
    uut.doWork( true, time );
    val.bk = 0; val.g = false; val.o = true; val.w1 = false; val.w2 = false, val.w3 = false; val.y1 = false, val.y2 = false;
    mp_relayOutputs.read( relays );
    REQUIRE( memcmp( &relays, &val, sizeof( val ) ) == 0 );
    mp_equipmentBeginTimes.read( equipTimes );
    REQUIRE( equipTimes.indoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.outdoorUnitBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.systemBeginOnTime == prevTime2 );
    REQUIRE( equipTimes.indoorUnitBeginOffTime == prevTime4 );
    REQUIRE( equipTimes.outdoorUnitBeginOffTime == prevTime3 );
    REQUIRE( equipTimes.systemBeginOffTime == prevTime6 );
    mp_systemOn.write( false );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

