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
#include "ScheduledSimHouse.h"
#include "Storm/Thermostat/Main/Main.h"
#include "Storm/Thermostat/Main/Private_.h"
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
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Cpl/System/Thread.h"
#include "Storm/Thermostat/Ui/PicoDisplay/Ui.h"
#include "Storm/Thermostat/Ui/PicoDisplay/LogicalButtons.h"

#define MY_APP_TRACE_SECTION    "storm"


////////////////////////////////////////////////////////////////////////////////
Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::PolledMaker                     cmdProcessor_( g_cmdlist );
static Cpl::TShell::Cmd::Help	                    helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye	                    byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace	                    traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint	                    tprintCmd_( g_cmdlist );
static Cpl::Dm::TShell::Dm	                        dmCmd_( g_cmdlist, g_modelDatabase );
static Storm::TShell::State	                        stateCmd_( g_cmdlist );
static Storm::TShell::User	                        userCmd_( g_cmdlist );
static Storm::TShell::Filter	                    filterCmd_( g_cmdlist );
static Storm::TShell::WhiteBox	                    whiteBoxCmd_( g_cmdlist );
static Cpl::Io::Input*                              consoleInputFdPtr_;
static Cpl::Io::Output*                             consoleOutputFdPtr_;


// The control algorithm
static Storm::Thermostat::Algorithm thermostatAlgorithm_( *g_algoRunnablePtr );

// Include the House simulation since we don't actual have (yet) a temperature sensor
static Storm::Thermostat::Main::ScheduledSimHouse houseSim_;

// For shutdown logic
static volatile int exitCode_;
static int runShutdownHandlers() noexcept;

// Allocate/create my Model Database
Cpl::Dm::ModelDatabase   g_modelDatabase( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );


/*-----------------------------------------------------------*/
//
// Set up Periodic scheduling: Core 0 
//

/// 100Hz
static void interval_10ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                           Cpl::System::ElapsedTime::Precision_T currentInterval,
                           void*                                 context_notUsed )
{
    Storm::Thermostat::Ui::PicoDisplay::processLogicalButtons( currentTick, currentInterval );
}

/// 10Hz
static void interval_100ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval,
                            void*                                 context_notUsed )
{
    Storm::Thermostat::Ui::PicoDisplay::processUI( currentTick, currentInterval );
}

/// 1Hz
static void interval_1000ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                             Cpl::System::ElapsedTime::Precision_T currentInterval,
                             void*                                 context_notUsed )
{
    houseSim_.scheduleSimulation();
}

// Forward References
static void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted );
static void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick );
static void core0Stop( Cpl::System::ElapsedTime::Precision_T currentTick );

// Periodic Intervals
static Cpl::System::PeriodicScheduler::Interval_T core0Intervals_[] =
{
    { interval_10ms, {0, 10}, nullptr },
    { interval_100ms, { 0, 100 }, nullptr  },
    { interval_1000ms, { 0, 1000 }, nullptr  },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Runnable object for thread
Cpl::Dm::PeriodicScheduler core0Mbox_( core0Intervals_,
                                       core0Start,
                                       core0Stop,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       core0Idle );
Cpl::Dm::PeriodicScheduler* g_uiRunnablePtr = &core0Mbox_;


// In thread initialization 
void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("core0Start") );
    openPlatform0();
    thermostatAlgorithm_.open();
    Storm::Thermostat::Ui::PicoDisplay::intializeLogicalButtons();
    Storm::Thermostat::Ui::PicoDisplay::intializeUI();
    cmdProcessor_.getCommandProcessor().start( *consoleInputFdPtr_, *consoleOutputFdPtr_, false ); 
}

// In thread shutdown 
void core0Stop( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("core0Stop") );
    
    // Reverse order of the start()
    cmdProcessor_.getCommandProcessor().requestStop();
    Storm::Thermostat::Ui::PicoDisplay::shutdownUI();
    Storm::Thermostat::Ui::PicoDisplay::shutdownLogicalButtons();
    thermostatAlgorithm_.close();
    closePlatform0();
    runShutdownHandlers();
}

void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    // Execute the Console shell
    cmdProcessor_.getCommandProcessor().poll();
}


/*-----------------------------------------------------------*/
int runTheApplication( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am Storm Thermostat!.") );

    // Cache the Console streams
    consoleInputFdPtr_  = &infd;
    consoleOutputFdPtr_ = &outfd;

    // Put the ModelPoints in their 'default' initialized state
    initializeModelPoints();

    // Finish any remaining platform specific stuff....
    initializePlatform0();

    // Create my threads
    CPL_SYSTEM_ASSERT( g_algoRunnablePtr );
    Cpl::System::Thread::create( core0Mbox_, "CORE0" );
    Cpl::System::Thread::create( *g_algoRunnablePtr, "ALGO" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
    return 0;   // Note: the function never exits
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
    core0Mbox_.pleaseStop();
    return OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE;
}

int Cpl::System::Shutdown::failure( int exit_code )
{
    exitCode_ = exit_code;
    core0Mbox_.pleaseStop();
    return exit_code;
}

