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
#include "Storm/Component/FanControl.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"


#define FAN_CONT_SPEED  499

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "FanControl" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    FanControl::Input_T  ins  = { &mp_fanMode,  &mp_systemConfig, &mp_vOutputs, &mp_equipmentBeginTimes };
    FanControl::Output_T outs = { &mp_vOutputs };

    Storm::Type::SystemConfig_T sysCfg;
    Storm::Dm::MpSystemConfig::setToOff( sysCfg );
    sysCfg.fanContinuousSpeed = FAN_CONT_SPEED;
    sysCfg.currentOpMode      = Storm::Type::OperatingMode::eOFF;
    mp_systemConfig.write( sysCfg );
    mp_fanMode.write( Storm::Type::FanMode::eAUTO );

    FanControl uut( ins, outs );

    // Start the components
    Cpl::System::ElapsedTime::Precision_T time = { 1, 0 };
    uut.start( time );

    // Fan Auto
    time.m_seconds += 1;
    uut.doWork( true, time );
    Storm::Type::VirtualOutputs_T  outputValues;
    mp_vOutputs.read( outputValues );
    REQUIRE( outputValues.indoorFanCont == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );

    // Fan Continuous 
    mp_fanMode.write( Storm::Type::FanMode::eCONTINUOUS );
    time.m_seconds += 1;
    uut.doWork( true, time );
    mp_vOutputs.read( outputValues );
    REQUIRE( outputValues.indoorFanCont == FAN_CONT_SPEED );

    // Fan Auto 
    mp_vOutputs.setIndoorFanOutput( STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );   // Simulate off mode actions
    mp_systemConfig.write( sysCfg );
    mp_fanMode.write( Storm::Type::FanMode::eAUTO );
    time.m_seconds += 1;
    uut.doWork( true, time );
    mp_vOutputs.read( outputValues );
    REQUIRE( outputValues.indoorFanCont == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

