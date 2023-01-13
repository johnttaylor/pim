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


// Create the Application Mailbox/EventLoop and the Bob module
static Cpl::Dm::MailboxServer       appMbox_;
static BobModule                    bob_( appMbox_ );

// Stuffs needed for receiving/handle the Shutdown signal (part of the CPL C++ class library)
static Cpl::System::Semaphore       waitForShutdown_;
static volatile int                 exitCode_;



/* NOTE: Because this example is target to run on a Desktop OS, numerous
         objects are created on the stack(e.g. 'cmdProccesor'). Where as an
         application that has to run on embedded target - these objects would
         be created statically or on the heap.
*/
int runTheApplication( MyShellOption_T  shellType,
                       Cpl::Io::Input&  infd,
                       Cpl::Io::Output& outfd )
{
    Cpl::System::Thread* appThread = nullptr;
    
    // Enable Trace 
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( BOB_SECT_ );

    // Create a BLOCKING TShell that run in a dedicate thread
    if ( shellType == SHELL_BLOCKING )
    {
        // Create the Application 
        Cpl::System::Thread* appThread = Cpl::System::Thread::create( appMbox_, "Application", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

        // Create a blocking TShell processor instance
        Cpl::TShell::Maker  cmdProcessor( g_cmdlist );

        // Create a helper object that manages a dedicated thread for a
        // TShell processor to execute in.  The thread is created when
        // shell.launch() is called.
        Cpl::TShell::Stdio   shell( cmdProcessor, "TShell", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER + CPL_SYSTEM_THREAD_PRIORITY_LOWER );


        // Start the application
        bob_.open();

        // Start the TShell/debug-console
        shell.launch( infd, outfd );

        // Wait for the Application to be shutdown
        waitForShutdown_.wait();

        // Shutdown application objects (MUST be done in the reverse order of the open() calls)
        bob_.close();

        // Shutdown the Application thread
        appMbox_.pleaseStop();
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
