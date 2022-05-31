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

#include "colony_config.h"
#include "Storm/Thermostat/_file_logger/Log.h"
#include "Storm/Thermostat/Algorithm.h"
#include "Storm/Thermostat/Outputs.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Storm/Thermostat/SimHouse/ModelPoints.h"
#include "Storm/Thermostat/SimHouse/Cmd.h"
#include "Storm/Thermostat/SimHouse/House.h"
#include "Storm/TShell/State.h"
#include "Storm/Utils/SimHouse.h"
#include "Cpl/TShell/Cmd/Tick.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/EventLoop.h"

/// 
extern void algorithmTest( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


////////////////////////////////////////////////////////////////////////////////
static Cpl::Container::Map<Cpl::TShell::Command>  cmdlist_( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Maker                         cmdProcessor_( cmdlist_ );
static Cpl::TShell::Stdio                         shell_( cmdProcessor_ );

static Cpl::TShell::Cmd::Tick	        tick_( cmdlist_ );
static Cpl::TShell::Cmd::Threads	    threads_( cmdlist_ );
static Cpl::TShell::Cmd::Help	        helpCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Bye	        byeCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Trace	        traceCmd_( cmdlist_ );
static Cpl::TShell::Cmd::TPrint	        tprintCmd_( cmdlist_ );
static Cpl::Dm::TShell::Dm	            dmCmd_( cmdlist_, g_modelDatabase, "dm" );
static Storm::TShell::State	            stateCmd_( cmdlist_ );
static Storm::Thermostat::SimHouse::Cmd houseCmd_( cmdlist_ );
static Storm::Thermostat::Log           logCmd_( cmdlist_ );

static Storm::Thermostat::Algorithm uut_;

static Storm::Thermostat::SimHouse::House houseSimulator_;

static void initializeModelPoints() noexcept;

void algorithmTest( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Start the shell
    shell_.launch( infd, outfd );

    // Create thread to run the Algorithm
    Cpl::System::Thread::create( uut_, "Algorithm", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );

    // Create thread to run the House simulation
    Cpl::System::Thread::create( houseSimulator_, "HouseSim", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );

    // Start the algorithm
    initializeModelPoints();
    uut_.open();

    // RUN.  Note: upon return, main() goes into a forever loop
}

#define SETPOINT_COOLING        77.0F
#define SETPOINT_HEATING        70.0F
#define INITIAL_PRIMARY_IDT     75.0F
#define INITIAL_SECONDARY_IDT   71.0F


void initializeModelPoints() noexcept
{
    mp_setpoints.write( SETPOINT_COOLING, SETPOINT_HEATING );
    mp_userMode.write( Storm::Type::ThermostatMode::eOFF );
    mp_fanMode.write( Storm::Type::FanMode::eAUTO );
    mp_primaryRawIdt.write( INITIAL_PRIMARY_IDT );
    mp_secondaryRawIdt.write( INITIAL_SECONDARY_IDT );
    mp_activeIdt.setInvalid();
    mp_relayOutputs.setSafeAllOff();
    mp_idtAlarms.setAlarm( false, false, false );
    mp_noActiveConditioningAlarm.setAlarm( false, false );
    mp_userCfgModeAlarm.setAlarm( false, false );
    mp_enabledSecondaryIdt.write( false );
    mp_equipmentConfig.writeCompressorStages( 1 );
    mp_equipmentConfig.writeIndoorFanMotor( false );
    mp_equipmentConfig.writeIndoorHeatingStages( 1 );
    mp_equipmentConfig.writeIndoorType( Storm::Type::IduType::eFURNACE );
    mp_equipmentConfig.writeOutdoorType( Storm::Type::OduType::eAC );
    Storm::Type::ComfortStageParameters_T configConfig = { 5 * 60, 5 * 50,  Storm::Type::Cph::e3CPH };
    mp_comfortConfig.writeCompressorCooling( configConfig );
    mp_comfortConfig.writeCompressorHeating( configConfig );
    mp_comfortConfig.writeIndoorHeating( configConfig );
    mp_systemForcedOffRefCnt.reset();
    mp_systemConfig.setInvalid();           // Algorithm will update this!
    mp_systemOn.write( false );
    mp_resetPiPulse.write( false );
    mp_operatingModeChanged.write( false );
    mp_deltaIdtError.write( 0.0F );
    mp_deltaSetpoint.write( 0.0F );
    mp_setpointChanged.write( false );
    mp_activeSetpoint.setInvalid();         // Algorithm will update this!    
    mp_freezePiRefCnt.reset();
    mp_inhibitfRefCnt.reset();
    mp_pvOut.write( 0.0F );
    mp_sumError.write( 0.0F );
    mp_pvInhibited.write( false );
    Storm::Type::VirtualOutputs_T zeroVOutputs = { 0, };
    mp_vOutputs.write( zeroVOutputs );
    Storm::Type::CycleInfo_T zeroCycleInfo;
    mp_cycleInfo.write( zeroCycleInfo );
    Storm::Type::EquipmentTimes_T zeroEquipmentBeginTimes;
    mp_equipmentBeginTimes.write( zeroEquipmentBeginTimes );

    mp_houseSimEnabled.write( false );
    mp_outdoorTemp.write( 70.0 );
}

void Storm::Thermostat::Outputs::updateHVACOutputs()
{
    // Do nothing
}


