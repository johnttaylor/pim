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
#include "Storm/Component/OperatingMode.h"
#include "Storm/Dm/MpVirtualOutputs.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Operating Mode" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    OperatingMode::Input_T  ins  = { &mp_setpoints,  &mp_userMode, &mp_activeIdt, &mp_equipmentBeginTimes, &mp_systemOn, &mp_systemForcedOffRefCnt, &mp_equipmentConfig, &mp_comfortConfig };
    OperatingMode::Output_T outs = { &mp_operatingModeChanged, &mp_resetPiPulse, &mp_systemForcedOffRefCnt, &mp_systemConfig, &mp_noActiveConditioningAlarm, &mp_userCfgModeAlarm };
    OperatingMode           component( ins, outs );

    // Default values...
    mp_userMode.write( Storm::Type::ThermostatMode::eOFF );
    mp_setpoints.write( 78.0F, 68.0F );
    mp_activeIdt.write( 80.F );
    mp_systemOn.write( false );
    mp_freezePiRefCnt.reset();
    mp_resetPiPulse.write( false );
    mp_systemForcedOffRefCnt.reset();

    // Start the component (and 'prime' it for the first real interval)
    Cpl::System::ElapsedTime::Precision_T time = { 1, 0 };
    component.start( time );
    component.doWork( true, time );
    time.m_seconds += 1;

    SECTION( "cooling/heating/off mode" )
    {
        mp_userMode.write( Storm::Type::ThermostatMode::eCOOLING );
        time.m_seconds += 1;
        component.doWork( true, time );

        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        bool changed;
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        bool reset;
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );

        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        mp_userMode.write( Storm::Type::ThermostatMode::eHEATING );
        time.m_seconds += 1;
        component.doWork( true, time );

        mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );

        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        mp_userMode.write( Storm::Type::ThermostatMode::eOFF );
        time.m_seconds += 1;
        component.doWork( true, time );

        mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );

        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );
    }

    SECTION( "bad inputs mode" )
    {
        mp_activeIdt.setInvalid();
        mp_userMode.write( Storm::Type::ThermostatMode::eCOOLING );
        time.m_seconds += 1;
        component.doWork( true, time );

        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        bool changed;
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        bool reset;
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );

        mp_resetPiPulse.write( false );
        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        mp_activeIdt.write( 80.0F );
        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );

        // Forced Off
        mp_systemForcedOffRefCnt.increment();
        mp_activeIdt.write( 80.0F );
        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );

        // Forced released
        mp_systemForcedOffRefCnt.reset();
        mp_activeIdt.write( 80.0F );
        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
    }

    SECTION( "auto mode - initial transitions" )
    {
        // Initial transition to auto (expected to resolve to cooling mode)
        mp_setpoints.write( 78.0F, 68.0F );
        mp_activeIdt.write( 70.0F );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        time.m_seconds += 1;
        component.doWork( true, time );

        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        bool changed;
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        bool reset;
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );

        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        // Turn off
        mp_userMode.write( Storm::Type::ThermostatMode::eOFF );
        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );

        // Initial transition to auto (expected to resolve to heating mode)
        mp_setpoints.write( 78.0F, 68.0F );
        mp_activeIdt.write( 68.0F );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        time.m_seconds += 1;
        component.doWork( true, time );

        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );

        time.m_seconds += 1;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );
    }

    SECTION( "auto mode - changeover to cooling" )
    {
        // Initial transition to auto (expected to resolve to heating mode)
        mp_setpoints.write( 78.0F, 68.0F );
        mp_activeIdt.write( 67.0F );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        time.m_seconds += 1;
        component.doWork( true, time );

        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        bool changed;
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        bool reset;
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );
        mp_equipmentBeginTimes.setSystemBeginOffTime( time );

        // Change the current temp to meet the criteria for switching to cooling (BUT not enough time has elapsed)
        mp_activeIdt.write( 78.0F );
        time.m_seconds += OPTION_STORM_COMPONENT_OPERATING_MODE_SECONDS_HYSTERESIS - 1;
        component.doWork( true, time );

        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        // Elapsed enough time for the change
        time.m_seconds += 1;
        component.doWork( true, time );

        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );
    }

    SECTION( "auto mode - changeover to heating" )
    {
        // Initial transition to auto (expected to resolve to cooling mode)
        mp_setpoints.write( 78.0F, 68.0F );
        mp_activeIdt.write( 72.0F );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        time.m_seconds += 1;
        component.doWork( true, time );

        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        bool changed;
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        bool reset;
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
        mp_resetPiPulse.write( false );
        mp_equipmentBeginTimes.setIndoorUnitBeginOffTime( time );

        // Change the current temp to ALMOST meet the criteria for switching to heating (BUT not enough time has elapsed)
        mp_activeIdt.write( 78.0F - ( OPTION_STORM_COMPONENT_OPERATING_MODE_COOLING_OFFSET / 2.0F ) );
        time.m_seconds += OPTION_STORM_COMPONENT_OPERATING_MODE_SECONDS_HYSTERESIS - 1;
        component.doWork( true, time );

        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        // Elapsed enough time for the change (but still not heating criteria)
        time.m_seconds += 1;
        component.doWork( true, time );

        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == false );

        // Meet the heating criteria)
        mp_activeIdt.write( 78.0F - OPTION_STORM_COMPONENT_OPERATING_MODE_COOLING_OFFSET * 1.5F );
        time.m_seconds += 1;
        component.doWork( true, time );

        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        valid = mp_operatingModeChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );
        valid = mp_resetPiPulse.read( reset );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( reset == true );
    }

    SECTION( "restricted operating modes" )
    {
        // Only Heating
        mp_equipmentConfig.writeCompressorStages( 0 );
        mp_userMode.write( Storm::Type::ThermostatMode::eCOOLING );
        time.m_seconds += 1;
        component.doWork( true, time );

        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        Storm::Dm::MpSimpleAlarm::Data alarm;
        valid = mp_systemConfig.read( sysCfg );
        mp_userCfgModeAlarm.read( alarm );
        REQUIRE( alarm.active == true );
        REQUIRE( alarm.critical == true );

        mp_equipmentConfig.writeCompressorStages( 1 );
        time.m_seconds += 1;
        component.doWork( true, time );

        sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        mp_userCfgModeAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( alarm.critical == false );

        // Only Cooling
        mp_equipmentConfig.writeIndoorHeatingStages( 0 );
        mp_userMode.write( Storm::Type::ThermostatMode::eHEATING );
        time.m_seconds += 1;
        component.doWork( true, time );

        sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        mp_userCfgModeAlarm.read( alarm );
        REQUIRE( alarm.active == true );
        REQUIRE( alarm.critical == true );

        mp_equipmentConfig.writeIndoorHeatingStages( 1 );
        time.m_seconds += 1;
        component.doWork( true, time );

        sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        mp_userCfgModeAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( alarm.critical == false );

        // Auto - Heating only
        mp_equipmentConfig.writeCompressorStages( 0 );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        time.m_seconds += 1;
        component.doWork( true, time );

        sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        mp_userCfgModeAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( alarm.critical == false );

        // Auto - Cooling only
        mp_equipmentConfig.writeCompressorStages( 1 );
        mp_equipmentConfig.writeIndoorHeatingStages( 0 );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        time.m_seconds += 1;
        component.doWork( true, time );

        sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        mp_userCfgModeAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( alarm.critical == false );
    }

    SECTION( "combinations..." )
    {
        // NO valid configuration
        mp_equipmentConfig.setInvalid();
        component.doWork( true, time );
        Storm::Type::SystemConfig_T sysCfg;
        int8_t valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eOFF );
        Storm::Dm::MpSimpleAlarm::Data alarm;
        valid = mp_noActiveConditioningAlarm.read( alarm );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( alarm.active == true );
        uint32_t refCount;
        mp_systemForcedOffRefCnt.read( refCount );
        REQUIRE( refCount > 0 );

        mp_setpoints.write( 78.0F, 68.0F );
        mp_activeIdt.write( 60.F );
        mp_userMode.write( Storm::Type::ThermostatMode::eAUTO );
        
        // cooling, Air Handler (no Electric Heat)
        Storm::Dm::MpEquipmentConfig::Data cfg = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 0, false };
        mp_equipmentConfig.write( cfg );
        time.m_seconds += 10 * 60;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        mp_noActiveConditioningAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eCOOLING );
        REQUIRE( sysCfg.numCompressorStages == 1 );
        REQUIRE( sysCfg.numIndoorStages == 0 );
        REQUIRE( sysCfg.totalStages == 1 );
        REQUIRE( sysCfg.fanContinuousSpeed == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_GAIN ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.reset, OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_RESET ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_GAIN ) );
        REQUIRE( sysCfg.outdoorUnitType == Storm::Type::OduType::eAC );
        REQUIRE( sysCfg.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER );
        mp_systemForcedOffRefCnt.read( refCount );
        REQUIRE( refCount == 0 );


        // cooling, Furnace/2 stages
        cfg = { Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 1, 2, true };
        mp_equipmentConfig.write( cfg );
        time.m_seconds += 10 * 60;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        mp_noActiveConditioningAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        REQUIRE( sysCfg.numCompressorStages == 0 );
        REQUIRE( sysCfg.numIndoorStages == 2 );
        REQUIRE( sysCfg.totalStages == 2 );
        REQUIRE( sysCfg.fanContinuousSpeed == (uint32_t)(OPTION_STORM_DEFAULT_VSP_BLOWER_FAN_CONT_SPEED* STORM_DM_MP_VIRTUAL_OUTPUTS_ON) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.reset, OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_RESET ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN ) );
        REQUIRE( sysCfg.outdoorUnitType == Storm::Type::OduType::eAC );
        REQUIRE( sysCfg.indoorUnitType == Storm::Type::IduType::eFURNACE );
        mp_systemForcedOffRefCnt.read( refCount );
        REQUIRE( refCount == 0 );

        // furnace/2 stage ONLY
        mp_activeIdt.write( 90.F );
        cfg = { Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 2, true };
        mp_equipmentConfig.write( cfg );
        time.m_seconds += 10 * 60;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        mp_noActiveConditioningAlarm.read( alarm );
        REQUIRE( alarm.active == false );
        REQUIRE( sysCfg.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING );
        REQUIRE( sysCfg.numCompressorStages == 0 );
        REQUIRE( sysCfg.numIndoorStages == 2 );
        REQUIRE( sysCfg.totalStages == 2 );
        REQUIRE( sysCfg.fanContinuousSpeed == ( uint32_t) ( OPTION_STORM_DEFAULT_VSP_BLOWER_FAN_CONT_SPEED * STORM_DM_MP_VIRTUAL_OUTPUTS_ON ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.reset, OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_RESET ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN ) );
        REQUIRE( sysCfg.outdoorUnitType == Storm::Type::OduType::eAC );
        REQUIRE( sysCfg.indoorUnitType == Storm::Type::IduType::eFURNACE );
        mp_systemForcedOffRefCnt.read( refCount );
        REQUIRE( refCount == 0 );

        // No Capacity
        cfg = { Storm::Type::IduType::eFURNACE, Storm::Type::OduType::eAC, 0, 0, true };
        mp_equipmentConfig.write( cfg );
        time.m_seconds += 10 * 60;
        component.doWork( true, time );
        valid = mp_systemConfig.read( sysCfg );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        Storm::Type::SystemConfig_T expectedVal;
        Storm::Dm::MpSystemConfig::setToOff( expectedVal );
        REQUIRE( sysCfg.currentOpMode == expectedVal.currentOpMode );
        REQUIRE( sysCfg.numCompressorStages == expectedVal.numCompressorStages );
        REQUIRE( sysCfg.numIndoorStages == expectedVal.numIndoorStages );
        REQUIRE( sysCfg.totalStages == expectedVal.totalStages );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, expectedVal.gain ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.reset, expectedVal.reset ) );
        REQUIRE( Cpl::Math::areFloatsEqual( sysCfg.gain, expectedVal.gain ) );
        REQUIRE( sysCfg.outdoorUnitType == expectedVal.outdoorUnitType );
        REQUIRE( sysCfg.indoorUnitType == expectedVal.indoorUnitType );
        mp_noActiveConditioningAlarm.read( alarm );
        REQUIRE( alarm.active == true );
        REQUIRE( alarm.critical == true );
        mp_systemForcedOffRefCnt.read( refCount );
        REQUIRE( refCount > 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

