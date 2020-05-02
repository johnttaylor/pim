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
#include "Storm/Component/Equipment/IndoorHeating.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "MyStage.h"


using namespace Storm::Component::Equipment;

#define SECT_       "_0test"



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Indoor Heating" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    MyStage stage1;
    MyStage stage2;
    MyStage stage3;
    IndoorHeating uut(stage1, stage2, stage3 );
    Storm::Component::Control::Equipment::Args_T args = { 0, };
    Storm::Dm::MpSystemConfig::setToOff( args.systemConfig );
    args.vOutputs.sovInHeating             = true;
    args.currentInterval                   = { 1, 0 };
    args.currentTick                       = { 1, 1 };
    args.systemOn                          = false;
    args.pvOut                             = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_THRESHOLD + 1.0F;
    args.systemConfig.stages[0].minOffTime = 60;
    args.systemConfig.stages[0].minOnTime  = 90;
    args.systemConfig.stages[1].minOffTime = 10;
    args.systemConfig.stages[1].minOnTime  = 30;
    args.systemConfig.stages[2].minOffTime = 20;
    args.systemConfig.stages[2].minOnTime  = 50;
    args.systemConfig.numCompressorStages  = 1;
    args.systemConfig.numIndoorStages      = 1;
    args.systemConfig.indoorUnitType       = Storm::Type::IduType::eFURNACE;
    args.systemConfig.outdoorUnitType      = Storm::Type::OduType::eAC;

    SECTION( "one stage" )
    {
        // start() and reset()
        REQUIRE( stage1.m_countRequestModeToOff == 0 );
        REQUIRE( stage2.m_countRequestModeToOff == 0 );
        REQUIRE( stage3.m_countRequestModeToOff == 0 );
        uut.start( { 0, 100 } );
        REQUIRE( stage1.m_countRequestModeToOff == 1 );
        REQUIRE( stage2.m_countRequestModeToOff == 1 );
        REQUIRE( stage3.m_countRequestModeToOff == 1 );
        uut.executeOff( args );
        REQUIRE( stage1.m_countRequestModeToOff == 2 );
        REQUIRE( stage2.m_countRequestModeToOff == 2 );
        REQUIRE( stage2.m_countRequestModeToOff == 2 );
        stage1.m_countRequestModeToOff = 0;
        stage2.m_countRequestModeToOff = 0;
        stage3.m_countRequestModeToOff = 0;

        // Transition from system off to system on
        REQUIRE( stage1.m_countExecute == 0 );
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC - 10, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 1 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 0 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC + 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 2 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = true;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = true;
        args.systemOn              = true;

        // Running - no change in stage(s)
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 3 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );

        // Transition from Running to off
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_THRESHOLD;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 4 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        REQUIRE( stage1.m_countRequestOff == 0 );
        args.currentInterval += { args.systemConfig.stages[0].minOnTime, 0 };
        uut.executeActive(  args );
        REQUIRE( stage1.m_countExecute == 5 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        REQUIRE( stage1.m_countRequestOff == 1 );
        stage1.m_isOnCycle  = false;
        stage1.m_isOffCycle = true;
        args.systemOn       = false;

        // Transition from system off to system on
        args.pvOut = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_THRESHOLD + 1.0F;
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC + 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 6 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 2 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.equipmentBeginTimes.indoorUnitBeginOnTime = args.currentInterval;
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = true;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = true;
        args.systemOn              = true;


        // Transition from Running to off (while in off cycle)
        stage1.m_isOnCycle    = false;
        stage1.m_isOffCycle   = true;
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_THRESHOLD - 1.0F;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 7 );
        REQUIRE( stage2.m_countExecute == 0 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 2 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        REQUIRE( stage1.m_countRequestOff == 2 );
        stage1.m_isOnCycle = false;
        args.systemOn      = false;
    }

    SECTION( "two stages" )
    {
        args.systemConfig.numIndoorStages = 2;
    
        // start() and reset()
        REQUIRE( stage1.m_countRequestModeToOff == 0 );
        REQUIRE( stage2.m_countRequestModeToOff == 0 );
        REQUIRE( stage3.m_countRequestModeToOff == 0 );
        uut.start( { 0, 100 } );
        REQUIRE( stage1.m_countRequestModeToOff == 1 );
        REQUIRE( stage2.m_countRequestModeToOff == 1 );
        REQUIRE( stage3.m_countRequestModeToOff == 1 );
        uut.executeOff( args );
        REQUIRE( stage1.m_countRequestModeToOff == 2 );
        REQUIRE( stage2.m_countRequestModeToOff == 2 );
        REQUIRE( stage2.m_countRequestModeToOff == 2 );
        stage1.m_countRequestModeToOff = 0;
        stage2.m_countRequestModeToOff = 0;
        stage3.m_countRequestModeToOff = 0;

        // Transition from system off to system on - first stage
        REQUIRE( stage1.m_countExecute == 0 );
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC - 10, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 1 );
        REQUIRE( stage2.m_countExecute == 1 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 0 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC + 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 2 );
        REQUIRE( stage2.m_countExecute == 2 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = true;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = true;
        args.systemOn              = true;

        // Running - no change in stage(s)
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 3 );
        REQUIRE( stage2.m_countExecute == 3 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );

        // Transition from 1st stage to the 2nd stage
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_2ND_STAGE_THRESHOLD + 1.0F;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 4 );
        REQUIRE( stage2.m_countExecute == 4 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = false;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = false;
        stage1.m_isSupplementing   = true;
        stage2.m_isOnCycle         = true;
        stage2.m_isOffCycle        = false;
        stage2.m_isActive          = true;

        // Running - no change in stage(s)
        args.currentInterval += { 100, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 5 );
        REQUIRE( stage2.m_countExecute == 5 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );

        // Transition from 2nd to 1st stage
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_2ND_STAGE_THRESHOLD - 1.0F;
        args.currentInterval += { args.systemConfig.stages[1].minOnTime, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 6 );
        REQUIRE( stage2.m_countExecute == 6 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOff == 0 );
        REQUIRE( stage2.m_countRequestOff == 1 );
        stage1.m_isOnCycle                              = false;
        stage1.m_isOffCycle                             = true;
        stage1.m_isActive                               = true;
        stage1.m_isSupplementing                        = false;
        stage2.m_isOnCycle                              = false;
        stage2.m_isOffCycle                             = false;
        stage2.m_isActive                               = false;
        args.equipmentBeginTimes.indoorUnitBeginOffTime = args.currentInterval;
        args.cycleInfo.beginOffTime                     = args.currentInterval;

        // Transition from Running to off (while in off cycle)
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_THRESHOLD - 1.0F;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 7 );
        REQUIRE( stage2.m_countExecute == 7 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOff == 1 );
        REQUIRE( stage2.m_countRequestOff == 1 );
        stage1.m_isOnCycle   = false;
        stage1.m_isOffCycle  = false;
        stage1.m_isActive    = false;
        args.systemOn        = false;

        // Transition from system off to system on - 2nd stage
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_2ND_STAGE_THRESHOLD + 1.0F;
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC + 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 8 );
        REQUIRE( stage2.m_countExecute == 8 );
        REQUIRE( stage3.m_countExecute == 0 );
        REQUIRE( stage1.m_countRequestOn == 2 );
        REQUIRE( stage1.m_countRequestAsSupplement == 2 );
        REQUIRE( stage1.m_countRequestOff == 1 );
        REQUIRE( stage2.m_countRequestOff == 1 );
    }

    SECTION( "third stages" )
    {
        args.systemConfig.numIndoorStages = 3;
    
        // start() and reset()
        REQUIRE( stage1.m_countRequestModeToOff == 0 );
        REQUIRE( stage2.m_countRequestModeToOff == 0 );
        REQUIRE( stage3.m_countRequestModeToOff == 0 );
        uut.start( { 0, 100 } );
        REQUIRE( stage1.m_countRequestModeToOff == 1 );
        REQUIRE( stage2.m_countRequestModeToOff == 1 );
        REQUIRE( stage3.m_countRequestModeToOff == 1 );
        uut.executeOff( args );
        REQUIRE( stage1.m_countRequestModeToOff == 2 );
        REQUIRE( stage2.m_countRequestModeToOff == 2 );
        REQUIRE( stage2.m_countRequestModeToOff == 2 );
        stage1.m_countRequestModeToOff = 0;
        stage2.m_countRequestModeToOff = 0;
        stage3.m_countRequestModeToOff = 0;

        // Transition from system off to system on - first stage
        REQUIRE( stage1.m_countExecute == 0 );
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC - 10, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 1 );
        REQUIRE( stage2.m_countExecute == 1 );
        REQUIRE( stage3.m_countExecute == 1 );
        REQUIRE( stage1.m_countRequestOn == 0 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC + 1, 0 };
        uut.executeActive(  args );
        REQUIRE( stage1.m_countExecute == 2 );
        REQUIRE( stage2.m_countExecute == 2 );
        REQUIRE( stage3.m_countExecute == 2 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = true;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = true;
        args.systemOn              = true;

        // Running - no change in stage(s)
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 3 );
        REQUIRE( stage2.m_countExecute == 3 );
        REQUIRE( stage3.m_countExecute == 3 );
        REQUIRE( stage1.m_countRequestOn == 1 );

        // Transition from 1st stage to the 2nd stage
        REQUIRE( stage1.m_countRequestAsSupplement == 0 );
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_2ND_STAGE_THRESHOLD + 1.0F;
        args.currentInterval += { 1, 0 };
        uut.executeActive(  args );
        REQUIRE( stage1.m_countExecute == 4 );
        REQUIRE( stage2.m_countExecute == 4 );
        REQUIRE( stage3.m_countExecute == 4 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = false;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = false;
        stage1.m_isSupplementing   = true;
        stage2.m_isOnCycle         = true;
        stage2.m_isOffCycle        = false;
        stage2.m_isActive          = true;

        // Running - no change in stage(s)
        args.currentInterval += { 100, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 5 );
        REQUIRE( stage2.m_countExecute == 5 );
        REQUIRE( stage3.m_countExecute == 5 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );

        // Transition from 2nd stage to the 3rd stage
        REQUIRE( stage2.m_countRequestAsSupplement == 0 );
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_3RD_STAGE_THRESHOLD + 1.0F;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 6 );
        REQUIRE( stage2.m_countExecute == 6 );
        REQUIRE( stage3.m_countExecute == 6 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage2.m_countRequestAsSupplement == 1 );
        args.cycleInfo.beginOnTime = args.currentInterval;
        stage1.m_isOnCycle         = false;
        stage1.m_isOffCycle        = false;
        stage1.m_isActive          = false;
        stage1.m_isSupplementing   = true;
        stage2.m_isOnCycle         = false;
        stage2.m_isOffCycle        = false;
        stage2.m_isActive          = false;
        stage2.m_isSupplementing   = true;
        stage3.m_isOnCycle         = true;
        stage3.m_isOffCycle        = false;
        stage3.m_isActive          = true;

        // Running - no change in stage(s)
        args.currentInterval += { 100, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 7 );
        REQUIRE( stage2.m_countExecute == 7 );
        REQUIRE( stage3.m_countExecute == 7 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage2.m_countRequestAsSupplement == 1 );

        // Transition from 3rd to 2nd stage
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_3RD_STAGE_THRESHOLD - 1.0F;
        args.currentInterval += { args.systemConfig.stages[2].minOnTime, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 8 );
        REQUIRE( stage2.m_countExecute == 8 );
        REQUIRE( stage3.m_countExecute == 8 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOff == 0 );
        REQUIRE( stage2.m_countRequestOff == 0 );
        REQUIRE( stage3.m_countRequestOff == 1 );
        stage1.m_isOnCycle                              = false;
        stage1.m_isOffCycle                             = true;
        stage1.m_isActive                               = true;
        stage1.m_isSupplementing                        = true;
        stage2.m_isSupplementing                        = false;
        stage2.m_isOnCycle                              = true;
        stage2.m_isOffCycle                             = false;
        stage2.m_isActive                               = true;
        stage3.m_isOnCycle                              = false;
        stage3.m_isOffCycle                             = false;
        stage3.m_isActive                               = false;
        args.equipmentBeginTimes.indoorUnitBeginOffTime = args.currentInterval;
        args.cycleInfo.beginOffTime                     = args.currentInterval;

        // Transition from 2nd to 1st stage
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_2ND_STAGE_THRESHOLD - 1.0F;
        args.currentInterval += { args.systemConfig.stages[1].minOnTime, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 9 );
        REQUIRE( stage2.m_countExecute == 9 );
        REQUIRE( stage3.m_countExecute == 9 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOff == 0 );
        REQUIRE( stage2.m_countRequestOff == 1 );
        REQUIRE( stage3.m_countRequestOff == 1 );
        stage1.m_isOnCycle                              = false;
        stage1.m_isOffCycle                             = true;
        stage1.m_isActive                               = true;
        stage1.m_isSupplementing                        = false;
        stage2.m_isSupplementing                        = false;
        stage2.m_isOnCycle                              = false;
        stage2.m_isOffCycle                             = false;
        stage2.m_isActive                               = false;
        stage3.m_isOnCycle                              = false;
        stage3.m_isOffCycle                             = false;
        stage3.m_isActive                               = false;
        args.equipmentBeginTimes.indoorUnitBeginOffTime = args.currentInterval;
        args.cycleInfo.beginOffTime                     = args.currentInterval;

        // Transition from Running to off (while in off cycle)
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_THRESHOLD - 1.0F;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 10 );
        REQUIRE( stage2.m_countExecute == 10 );
        REQUIRE( stage3.m_countExecute == 10 );
        REQUIRE( stage1.m_countRequestOn == 1 );
        REQUIRE( stage1.m_countRequestAsSupplement == 1 );
        REQUIRE( stage1.m_countRequestOff == 1 );
        REQUIRE( stage2.m_countRequestOff == 1 );
        REQUIRE( stage3.m_countRequestOff == 1 );
        stage1.m_isOnCycle   = false;
        stage1.m_isOffCycle  = false;
        stage1.m_isActive    = false;
        args.systemOn        = false;

        // Transition from system off to system on - 3rd stage
        args.pvOut            = OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_3RD_STAGE_THRESHOLD + 1.0F;
        args.currentInterval += { OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC + 1, 0 };
        uut.executeActive(  args );
        REQUIRE( stage1.m_countExecute == 11 );
        REQUIRE( stage2.m_countExecute == 11 );
        REQUIRE( stage3.m_countExecute == 11 );
        REQUIRE( stage1.m_countRequestOn == 2 );
        REQUIRE( stage1.m_countRequestAsSupplement == 2 );
        REQUIRE( stage1.m_countRequestOff == 1 );
        REQUIRE( stage2.m_countRequestOff == 1 );
        stage1.m_isOnCycle        = true;
        stage1.m_isOffCycle       = false;
        stage1.m_isActive         = false;
        stage1.m_isSupplementing  = true;
        stage2.m_isOnCycle        = true;
        stage2.m_isOffCycle       = false;
        stage2.m_isActive         = true;
        stage2.m_isSupplementing  = false;
        args.systemOn             = true;
        args.currentInterval += { 1, 0 };
        uut.executeActive( args );
        REQUIRE( stage1.m_countExecute == 12 );
        REQUIRE( stage2.m_countExecute == 12 );
        REQUIRE( stage3.m_countExecute == 12 );
        REQUIRE( stage1.m_countRequestOn == 2 );
        REQUIRE( stage1.m_countRequestAsSupplement == 2 );
        REQUIRE( stage2.m_countRequestAsSupplement == 2 );
        REQUIRE( stage1.m_countRequestOff == 1 );
        REQUIRE( stage2.m_countRequestOff == 1 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
