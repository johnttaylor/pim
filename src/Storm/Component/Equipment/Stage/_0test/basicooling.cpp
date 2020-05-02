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
#include "Storm/Component/Equipment/Stage/BasicCooling.h"


using namespace Storm::Component::Equipment::Stage;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Basic Cooling" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Storm::Component::Control::Equipment::Args_T args = { 0, };
    BasicCooling uut;
    BasicCooling stage2( 1, 1 );
    args.systemConfig.stages[0].cph        = Storm::Type::Cph::e3CPH;
    args.systemConfig.stages[0].minOffTime = 302;
    args.systemConfig.stages[0].minOnTime  = 304;
    args.systemConfig.stages[0].lowerBound = 0.0F;
    args.systemConfig.stages[0].upperBound = 100.0F;
    args.systemConfig.stages[1].cph        = Storm::Type::Cph::e2CPH;
    args.systemConfig.stages[1].minOffTime = 350;
    args.systemConfig.stages[1].minOnTime  = 150;
    args.systemConfig.stages[1].lowerBound = 100.0F;
    args.systemConfig.stages[1].upperBound = 200.0F;

    SECTION( "FSM transitions1" )
    {
        // Reset/Init
        REQUIRE( uut.isInOff() );
        REQUIRE( uut.isActive() == false );

        // Start in an Off Cycle
        args.pvOut = 99.99F;
        args.systemOn = true;
        uut.requestOn( args, false );
        REQUIRE( uut.isActive() );
        REQUIRE( uut.isInOffCycle() );
        REQUIRE( args.cycleInfo.mode == Storm::Type::CycleStatus::eOFF_CYCLE );
        REQUIRE( args.cycleInfo.offTime == args.systemConfig.stages[0].minOffTime );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.vOutputs.sovInHeating == false );
        REQUIRE( args.systemOn == true );

        // No load
        uut.requestOff( args );
        REQUIRE( uut.isActive() == false );
        REQUIRE( uut.isOff() );
        REQUIRE( args.systemOn == false );

        // Start in an On Cycle
        args.pvOut = 0.1F;
        args.systemOn = true;
        uut.requestOn( args, true );
        REQUIRE( uut.isActive() );
        REQUIRE( uut.isInOnCycle() );
        REQUIRE( args.cycleInfo.mode == Storm::Type::CycleStatus::eON_CYCLE );
        REQUIRE( args.cycleInfo.onTime == args.systemConfig.stages[0].minOnTime );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.sovInHeating == false );
        REQUIRE( args.systemOn == true );

        // Transition from On to Off cycle
        args.currentInterval += { args.systemConfig.stages[0].minOnTime + 1, 0 };
        uut.execute( args );
        REQUIRE( uut.isActive() );
        REQUIRE( uut.isInOffCycle() );
        REQUIRE( args.cycleInfo.mode == Storm::Type::CycleStatus::eOFF_CYCLE );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.vOutputs.sovInHeating == false );
        REQUIRE( args.systemOn == true );

        // Transition from Off to On cycle
        args.currentInterval += { args.cycleInfo.offTime+ 1, 0 };
        uut.execute( args );
        REQUIRE( uut.isActive() );
        REQUIRE( uut.isInOnCycle() );
        REQUIRE( args.cycleInfo.mode == Storm::Type::CycleStatus::eON_CYCLE );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.sovInHeating == false );
        REQUIRE( args.systemOn == true );

        // Transition to system off
        uut.requestOff( args );
        REQUIRE( uut.isActive() == false );
        REQUIRE( uut.isOff() );
        REQUIRE( args.systemOn == false );
    }

    SECTION( "FSM transitions2" )
    {
        // Reset/Init
        REQUIRE( uut.isInOff() );
        REQUIRE( uut.isActive() == false );

        // Start in an On Cycle
        args.pvOut = 50.0F;
        args.systemOn = true;
        uut.requestOn( args, true );
        REQUIRE( uut.isActive() );
        REQUIRE( uut.isInOnCycle() );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[1] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.systemOn == true );

        // Goto supplementing (start next stage in an off cycle)
        REQUIRE( stage2.isOff() == true );
        args.pvOut = 120.0F;
        uut.requestAsSupplement( args, stage2, false );
        REQUIRE( uut.isSupplementing() == true );
        REQUIRE( stage2.isActive() == true );
        REQUIRE( stage2.isInOffCycle() == true );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[1] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.systemOn == true );

        // Transition back to the 1st stage (and start in an off cycle)
        args.pvOut = 50.0F;
        stage2.requestOff( args, false );
        REQUIRE( stage2.isOff() == true );
        REQUIRE( uut.isActive() == true );
        REQUIRE( uut.isInOffCycle() == true );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.vOutputs.outdoorStages[1] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.systemOn == true );

        // Goto supplementing (start next stage in an ON cycle)
        REQUIRE( stage2.isOff() == true );
        args.pvOut = 120.0F;
        uut.requestAsSupplement( args, stage2, true );
        REQUIRE( uut.isSupplementing() == true );
        REQUIRE( stage2.isActive() == true );
        REQUIRE( stage2.isInOnCycle() == true );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[1] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.systemOn == true );

        // Transition back to the 1st stage (and start in an ON cycle)
        args.pvOut = 50.0F;
        stage2.requestOff( args, true );
        REQUIRE( stage2.isOff() == true );
        REQUIRE( uut.isActive() == true );
        REQUIRE( uut.isInOnCycle() == true );
        REQUIRE( args.vOutputs.indoorFan == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[0] == STORM_DM_MP_VIRTUAL_OUTPUTS_ON );
        REQUIRE( args.vOutputs.outdoorStages[1] == STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        REQUIRE( args.systemOn == true );

        // Transition to system off
        uut.requestOff( args );
        REQUIRE( uut.isActive() == false );
        REQUIRE( uut.isOff() );
        REQUIRE( args.systemOn == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

