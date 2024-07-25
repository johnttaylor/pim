/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Cpl/System/Trace.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/TShell/Stdio.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Cpl/MApp/Cmd.h"
#include "Cpl/MApp/Manager.h"
#include "Cpl/MApp/Temperature/Api.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Shutdown.h"



#define SECT_     "_0test"

/// 
using namespace Cpl::MApp;



/// 
extern void runtest( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


extern Cpl::Container::Map<Cpl::TShell::Command>   cmdlist;

static Cpl::TShell::Maker       cmdProcessor_( cmdlist );
static Cpl::TShell::Stdio       shell_( cmdProcessor_ );

static Cpl::TShell::Cmd::Help	helpCmd_( cmdlist );
static Cpl::TShell::Cmd::Bye	byeCmd_( cmdlist );
static Cpl::TShell::Cmd::Trace	traceCmd_( cmdlist );
static Cpl::TShell::Cmd::TPrint	tprintCmd_( cmdlist );

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::Mp::Float        mp_apple_( modelDb_, "apple" );
static Cpl::Dm::Mp::Float        mp_orange_( modelDb_, "orange" );
static Cpl::Dm::Mp::Float        mp_cherry_( modelDb_, "cherry" );

static Cpl::Dm::TShell::Dm	    dmCmd_( cmdlist, modelDb_ );


static Cpl::Dm::MailboxServer   mappMbox_;

static Cpl::Container::Map<Cpl::MApp::MAppApi>   mappList_;
static Cpl::MApp::Manager                        mappManager_( mappMbox_, mappList_ );
static Cpl::MApp::Cmd                            mappCmd_( cmdlist, mappManager_ );

static Cpl::MApp::Temperature::Api               t1_( mappList_, mappMbox_, mp_apple_, "t1" );
static Cpl::MApp::Temperature::Api               t2_( mappList_, mappMbox_, mp_orange_, "t2" );
static Cpl::MApp::Temperature::Api               t3_( mappList_, mappMbox_, mp_cherry_, "t3" );


////////////////////////////////////////////////////////////////////
static Cpl::System::Semaphore shutdownWaiter_;
static int                    exitCode_;
static void waitForShutdown();

////////////////////////////////////////////////////////////////////////////////


void runtest( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Enable MApp/Trace output
    CPL_SYSTEM_TRACE_ENABLE_SECTION( OPTION_CPL_MAPP_TRACE_SECTION );

    // Create thread for the MApp instances to run in
    Cpl::System::Thread* t1 =  Cpl::System::Thread::create( mappMbox_, "MAPPS" );

    // Initialize some (but not all) MPs
    mp_apple_.write( 3.14F );
    mp_orange_.write( 75.0F );

    // Start the Manager
    mappManager_.open();

    // Start the shell
    shell_.launch( infd, outfd );

    // Wait for shutdown (aka issue the 'bye app' command in the console)
    waitForShutdown();

    // stop the Manager
    mappManager_.close();

    // Stop the MApp thread
    mappMbox_.pleaseStop();
    Cpl::System::Thread::destroy( *t1 );

    // Terminate the application
    exit( exitCode_ );
}

//////////////////////////////////////////////////////////////////////
void waitForShutdown()
{
    // Wait till I am signaled 
    shutdownWaiter_.wait();
}

static int shutdown_application_( int exit_code )
{
    exitCode_ = exit_code;
    shutdownWaiter_.signal();
    return exitCode_;
}

int Cpl::System::Shutdown::success( void )
{
    return shutdown_application_( notifyShutdownHandlers_( OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE ) );
}

int Cpl::System::Shutdown::failure( int exit_code )
{
    return shutdown_application_( notifyShutdownHandlers_( exit_code ) );
}
