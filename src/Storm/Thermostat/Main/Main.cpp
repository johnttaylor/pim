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
#include "Main.h"
#include "Private_.h"
#include "Cpl/System/Shutdown.h"
#include "Storm/Thermostat/Algorithm.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Storm/TShell/State.h"
#include "Storm/TShell/User.h"
#include "Storm/TShell/WhiteBox.h"
#include "Storm/TShell/Filter.h"
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
#include "Cpl/System/Semaphore.h"



////////////////////////////////////////////////////////////////////////////////
Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Maker                           cmdProcessor_( g_cmdlist );
static Cpl::TShell::Stdio                           shell_( cmdProcessor_, "TShell", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER + CPL_SYSTEM_THREAD_PRIORITY_LOWER );
static Cpl::TShell::Cmd::Help	                    helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye	                    byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace	                    traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint	                    tprintCmd_( g_cmdlist );
static Cpl::Dm::TShell::Dm	                        dmCmd_( g_cmdlist, g_modelDatabase );
static Storm::TShell::State	                        stateCmd_( g_cmdlist);
static Storm::TShell::User	                        userCmd_( g_cmdlist );
static Storm::TShell::Filter	                    filterCmd_( g_cmdlist );
static Storm::TShell::WhiteBox	                    whiteBoxCmd_( g_cmdlist );

static Storm::Thermostat::Algorithm thermostatAlgorithm_;

static Cpl::System::Semaphore       waitForShutdown_;
static volatile int                 exitCode_;

static void initializeModelPoints() noexcept;
static int runShutdownHandlers() noexcept;


// Allocate/create my Model Database
// NOTE: For the MickySoft compiler I must 'allocate' the g_modelDatabase before any
//       model points get instantiated.  By placing the allocation in the Main 
//       directory AND by using nqbp's 'firstObjects' feature (and setting the Main
//       directory to be a 'firstObjects') it seems to appease the MS gods.
//
//       Updated: The issue appears that the g_modelDatabases class's vtable
//       pointer was zero when static MP instances self registered with the
//       database.  As to why the vtable pointer is/was zero - is still a mystery
//       (the issue also appeared with the gcc/mingw compiler). The work-around 
//       was to make the insert_() method a non-virtual method.
Cpl::Dm::ModelDatabase   g_modelDatabase( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

int runTheApplication( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Put the ModelPoints in their 'default' initialized state
    initializeModelPoints();

    // Finish any remaining platform specific stuff....
    initializePlatform0();

    // Start the shell
    shell_.launch( infd, outfd );

    // Create thread to run the Algorithm
    Cpl::System::Thread::create( thermostatAlgorithm_, "Algorithm", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );

    // Start the application
    openPlatform0();
    thermostatAlgorithm_.open();


    // Wait for the Application to be shutdown
    waitForShutdown_.wait();


    // Shutdown application objects (MUST be done in the reverse order of the open() calls)
    thermostatAlgorithm_.close();
    closePlatform0();

    // Run any/all register shutdown handlers (as registered by the Cpl::System::Shutdown interface) and then exit
    return runShutdownHandlers();
}


#define INITIAL_PRIMARY_IDT     75.0F
#define INITIAL_SECONDARY_IDT   71.0F

void initializeModelPoints() noexcept
{
    mp_primaryRawIdt.write( INITIAL_PRIMARY_IDT );
    mp_secondaryRawIdt.write( INITIAL_SECONDARY_IDT );
    mp_activeIdt.setInvalid();
    mp_relayOutputs.setSafeAllOff();
    mp_idtAlarms.setAlarm( false, false, false );
    mp_noActiveConditioningAlarm.setAlarm( false, false );
    mp_userCfgModeAlarm.setAlarm( false, false );
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
    Storm::Type::CycleInfo_T zeroCycleInfo                = { 0, };
    mp_cycleInfo.write( zeroCycleInfo );
    Storm::Type::EquipmentTimes_T zeroEquipmentBeginTimes = { 0, };
    mp_equipmentBeginTimes.write( zeroEquipmentBeginTimes );
}



////////////////////////////////////////////////////////////////////////////////
int runShutdownHandlers() noexcept
{
    exitCode_ = exitPlatform( Cpl::System::Shutdown::notifyShutdownHandlers_( exitCode_ ) );
    return exitPlatform( exitCode_ );
}

int Cpl::System::Shutdown::success( void )
{
    exitCode_ = OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE;
    waitForShutdown_.signal();
    return OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE;
}

int Cpl::System::Shutdown::failure( int exit_code )
{
    exitCode_ = exit_code;
    waitForShutdown_.signal();
    return exit_code;
}
