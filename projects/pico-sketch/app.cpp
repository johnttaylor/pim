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

#include "Driver/PicoDisplay/Api.h" // Must be first #include (because of the Pimoroni/Pico SDK)
#include "app.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Driver/Button/PolledDebounced.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "ModelPoints.h"
#include "LogicalButtons.h"
#include "Sketch.h"

/*-----------------------------------------------------------*/

// Create the Command/Debug Console and populate with commands
Cpl::Container::Map<Cpl::TShell::Command>    g_cmdlist( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::TShell::PolledMaker              cmdProcessor_( g_cmdlist );
static Cpl::TShell::Cmd::Help	             helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye	             byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace	             traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint	             tprintCmd_( g_cmdlist );
static Cpl::Dm::TShell::Dm                   dmCmd_( g_cmdlist, mp::modelDatabase );




/*-----------------------------------------------------------*/
/// 100Hz
static void interval_10ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                           Cpl::System::ElapsedTime::Precision_T currentInterval,
                           void*                                 context_notUsed )
{
    // Process the buttons
    processLogicalButtons( currentTick, currentInterval );
}

/// 10Hz
static void interval_100ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval,
                            void*                                 context_notUsed )
{
    // Process the UI
    processUI( currentTick, currentInterval );
}

/*-----------------------------------------------------------*/
//
// Set up Periodic schedulingL Core 0 
//

// Forward References
static void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted );
static void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick );

// Periodic Intervals
static Cpl::System::PeriodicScheduler::Interval_T core0Intervals_[] =
{
    { interval_10ms, { 0, 10 }, nullptr  },
    { interval_100ms, { 0, 100 }, nullptr  },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Runnable object for thread
Cpl::Dm::PeriodicScheduler core0Mbox_( core0Intervals_,
                                       core0Start,
                                       nullptr,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       core0Idle );

Cpl::Dm::PeriodicScheduler* g_uiRunnablePtr = &core0Mbox_;


// In thread initialization 
void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    // Initialize sub-systems, drivers, etc.
    intializeLogicalButtons();
    intializeUI();

    Driver::PicoDisplay::Api::setLCDBrightness( 200 );
}

void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
}


/*-----------------------------------------------------------*/
//
// Set up Periodic schedulingL Core 1
//

// Forward References
static void core1Start( Cpl::System::ElapsedTime::Precision_T currentTick );
static void core1Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted );

// Periodic Intervals
static Cpl::System::PeriodicScheduler::Interval_T core1Intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Runnable object for Core1 (aka the thread's 'main loop')
Cpl::Dm::PeriodicScheduler core1Mbox_( core1Intervals_,
                                       core1Start,
                                       nullptr,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       core1Idle );

// In thread initialization 
static void core1Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    cmdProcessor_.getCommandProcessor().start( *g_consoleInputFdPtr, *g_consoleOutputFdPtr ); // Note: I don't need to set the 'blocking flag' because the processor knows it is non-blocking processor
}

static void core1Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    cmdProcessor_.getCommandProcessor().poll();
}


/*-----------------------------------------------------------*/
void runApplication() noexcept
{
    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am the Pico-Sketch App.") );

    // Create main application thread 
    Cpl::System::Thread::create( core0Mbox_, "APP" );

    // Create a 'Scheduler' thread for the second thread
    Cpl::System::Thread::create( core1Mbox_, "CORE2" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
}