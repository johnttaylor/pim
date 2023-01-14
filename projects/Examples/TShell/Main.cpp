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

#include "colony_config.h"
#include "Main.h"
#include "BobCommand.h"
#include "BobModule.h"
#include "ModelPoints.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/TShell/Socket.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/EventLoop.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/Dm/PeriodicScheduler.h"

////////////////////////////////////////////////////////////////////////////////
// Create the TShell command list and commands 
// Note: A platform specific 'thread' command is also created (but is created
//       in platform specific source file)
Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::Maker                           cmdProcessor_( g_cmdlist );
static Cpl::TShell::Cmd::Help                       helpCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Bye                        byeCmd_( g_cmdlist );
static Cpl::TShell::Cmd::Trace                      traceCmd_( g_cmdlist );
static Cpl::TShell::Cmd::TPrint                     tprintCmd_( g_cmdlist );
static Cpl::Dm::TShell::Dm                          dmCmd_( g_cmdlist, mp::g_modelDatabase );
static BobCommand                                   bobCmd_( g_cmdlist );

// Create a BLOCKING TShell processor instance
static Cpl::TShell::Maker  blockingCmdProcessor( g_cmdlist );

// Create a NON-BLOCKING TShell and supporting objects/functions when using 
// Periodic Scheduler within a an Event-based thread.
static Cpl::TShell::PolledMaker polledCmdProcessor_( g_cmdlist );



///////////////////////////////////////////////////////////////////////////////
// Support objects/function when using Periodic scheduling (i.e. using a Non-blocking
// TShell)

// Callback function to provide the TShell a timeslice when using a Periodic Scheduler
static void tshellScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    polledCmdProcessor_.getCommandProcessor().poll();
}

// Callback function for 'scheduled' entities. Not used in this example, but
// need an 'empty' list to satisfy the Periodic Scheduler interface
static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Callback function that is called at the before the event loop is started for
// the application thread
static Cpl::Io::Input*  infd_;
static Cpl::Io::Output* outfd_;
static void periodicSchedularThreadInit( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    CPL_SYSTEM_ASSERT( infd_ );
    CPL_SYSTEM_ASSERT( outfd_ );
    polledCmdProcessor_.getCommandProcessor().start( *infd_, *outfd_ ); // Note: I don't need to set the 'blocking flag' because the processor knows it is non-blocking processor
}

// Create a event-based thread WITH periodic scheduling.  Note: the TShell is
// executed in the 'idle' time-slice of the periodic scheduler.
static Cpl::Dm::PeriodicScheduler appSheduler_( intervals_, periodicSchedularThreadInit, nullptr, nullptr, Cpl::System::ElapsedTime::precision, tshellScan_ );


///////////////////////////////////////////////////////////////////////////////
// Stuffs needed for receiving/handle the Shutdown signal (part of the CPL C++ class library)
static Cpl::System::Semaphore       waitForShutdown_;
static volatile int                 exitCode_;



/* NOTE: Because this example is targeted to run on a Desktop OS, numerous
         objects are created on the stack(e.g. 'appMbox'). Where as an
         application that has to run on embedded target - these objects would
         be created statically or on the heap.
*/
int runTheApplication( MyShellOption_T              shellType,
                       Cpl::Io::Input&              infd,
                       Cpl::Io::Output&             outfd,
                       Cpl::Io::Socket::Listener&   listener,
                       int                          portNum )
{
    // Enable Trace 
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( BOB_SECT_ );

    //
    // Create a BLOCKING TShell that runs in a dedicate thread
    //
    if ( shellType == SHELL_BLOCKING )
    {
        // Create the 'Application'
        Cpl::Dm::MailboxServer  appMbox;
        BobModule               bob( appMbox );
        Cpl::System::Thread*    appThread = Cpl::System::Thread::create( appMbox, "Application", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

        // Create a helper object that manages a dedicated thread for a
        // TShell processor to execute in.  The thread is created when
        // shell->launch() is called.
        Cpl::TShell::Stdio* shell = new(std::nothrow) Cpl::TShell::Stdio( blockingCmdProcessor, "TShell", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER + CPL_SYSTEM_THREAD_PRIORITY_LOWER );
        CPL_SYSTEM_ASSERT( shell );

        // Start the application
        bob.open();

        // Start the TShell/debug-console
        shell->launch( infd, outfd );

        // Wait for the Application to be shutdown
        waitForShutdown_.wait();

        // Shutdown application objects (MUST be done in the reverse order of the open() calls)
        bob.close();

        // Shutdown the Application thread
        delete shell;
        appMbox.pleaseStop();
        Cpl::System::Thread::destroy( *appThread );
    }

    //
    // Create a BLOCKING TShell that runs in the TCP Socket Listener thread.
    //
    else if ( shellType == SHELL_SOCKETS)
    {
        // Create the 'Application'
        Cpl::Dm::MailboxServer  appMbox;
        BobModule               bob( appMbox );
        Cpl::System::Thread*    appThread = Cpl::System::Thread::create( appMbox, "Application", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

        // Create a helper object that is a TCP Socket Listener running in its
        // own thread.  The TShell process executes in the context of the Listener's
        // thread. The thread is created when shell->launch() is called.
        Cpl::TShell::Socket* shell = new(std::nothrow) Cpl::TShell::Socket( blockingCmdProcessor, listener, "TShell", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER + CPL_SYSTEM_THREAD_PRIORITY_LOWER );
        CPL_SYSTEM_ASSERT( shell );

        // Start the application
        bob.open();

        // Start the TShell/debug-console
        shell->launch( portNum );

        // Wait for the Application to be shutdown
        waitForShutdown_.wait();

        // Shutdown application objects (MUST be done in the reverse order of the open() calls)
        bob.close();

        // Shutdown the Application thread
        delete shell;
        appMbox.pleaseStop();
        Cpl::System::Thread::destroy( *appThread );
    }

    //
    // Create a NON-BLOCKING TShell that runs in the Application thread
    //
    if ( shellType == SHELL_NON_BLOCKING )
    {
        // Configure/set IO Streams for the non-blocking tshell
        infd_  = &infd;
        outfd_ = &outfd;

        // Create the 'Application'
        // NOTE: The TShell is started when the application thread starts executing.
        Cpl::Dm::PeriodicScheduler appMboxWithPeriodicScheduling( intervals_, periodicSchedularThreadInit, nullptr, nullptr, Cpl::System::ElapsedTime::precision, tshellScan_ );
        BobModule                  bob( appMboxWithPeriodicScheduling );
        Cpl::System::Thread*       appThread = Cpl::System::Thread::create( appMboxWithPeriodicScheduling, "Application", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
        
        // Start the application
        bob.open();

        waitForShutdown_.wait();

        // Shutdown application objects (MUST be done in the reverse order of the open() calls)
        bob.close();

        // Shutdown the Application thread
        appMboxWithPeriodicScheduling.pleaseStop();
        Cpl::System::Thread::destroy( *appThread );
    }

    return 0;
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
