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
#include "InputDriver.h"
#include "OutputDriver.h"
#include "Algorithm.h"
#include "ModelPoints.h"
#include "Cpl/System/Shutdown.h"
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

// Create the TShell (aka the debug console) and populate with the basic set 
// of commands
Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Maker                           cmdProcessor_( g_cmdlist );
static Cpl::TShell::Stdio                           shell_( cmdProcessor_, "TShell", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER );
static Cpl::TShell::Cmd::Help                       helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye                        byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace                      traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint                     tprintCmd_( g_cmdlist );
static Cpl::Dm::TShell::Dm                          dmCmd_( g_cmdlist, mp::g_modelDatabase );

// Create the Application Mailbox/EventLoop and services
static Cpl::Dm::MailboxServer       appMbox_;
static Algorithm algo_( appMbox_, mp::signalIn, mp::hiAlarm, mp::loAlarm, mp::metrics );

// Create the Driver Mailbox/EventLoop and services
static Cpl::Dm::MailboxServer       driverMbox_;
static InputDriver                  inDriver_( driverMbox_, mp::signalIn );
static OutputDriver                 outDriver_( driverMbox_, mp::hiAlarm, mp::loAlarm );

// Stuffs needed for receiving/handle the Shutdown signal (part of the CPL C++ class library)
static Cpl::System::Semaphore       waitForShutdown_;
static volatile int                 exitCode_;



int runTheApplication( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Enable Trace 
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "Algorithm" );
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "OutputDriver" );

    // Create the Application and Driver threads
    Cpl::System::Thread::create( appMbox_, "Application", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
    Cpl::System::Thread::create( driverMbox_, "Drivers", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_RAISE );

    // Start the TShell/debug-console
    shell_.launch( infd, outfd );

    // Start the application
    outDriver_.open();
    inDriver_.open();
    algo_.open();

    // Wait for the Application to be shutdown
    waitForShutdown_.wait();

    // Shutdown application objects (MUST be done in the reverse order of the open() calls)
    algo_.close();
    inDriver_.close();
    outDriver_.close();

    // Run any/all register shutdown handlers (as registered by the Cpl::System::Shutdown interface) and then exit
    return Cpl::System::Shutdown::notifyShutdownHandlers_( exitCode_ );
}

////////////////////////////////////////////////////////////////////////////////
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
