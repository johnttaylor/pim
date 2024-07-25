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
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/PeriodicScheduler.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Driver/DIO/InOut.h"
#include "Driver/DIO/_tshell/InOutCmd.h"

/// 
extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );
///
#define SECT_       "_0test"


////////////////////////////////////////////////////////////////////////////////

/// 
extern Cpl::Container::Map<Cpl::TShell::Command>    cmdlist;
static Cpl::TShell::PolledMaker                     polledCmdProcessor_( cmdlist );

//////////////////////////////////////////////
static void tshellScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    polledCmdProcessor_.getCommandProcessor().poll();
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - actual interval not needed for the test
};

static Cpl::Io::Input*  infd_;
static Cpl::Io::Output* outfd_;

static void initTShellThread( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    polledCmdProcessor_.getCommandProcessor().start( *infd_, *outfd_ );
}

static Cpl::Dm::PeriodicScheduler scheduler_( intervals_, initTShellThread, nullptr, nullptr, Cpl::System::ElapsedTime::precision, tshellScan_ );


////////////////////////////////////////////////////////////////////////////////

static Cpl::TShell::Cmd::Help	helpCmd_( cmdlist );
static Cpl::TShell::Cmd::Bye	byeCmd_( cmdlist );
static Cpl::TShell::Cmd::Trace	traceCmd_( cmdlist );
static Cpl::TShell::Cmd::TPrint	tprintCmd_( cmdlist );

static Driver::DIO::InOutCmd driverCmd_( cmdlist );


void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting Test...") );

    infd_  = &infd;
    outfd_ = &outfd;

    // Create a 'Scheduler' thread for the TShell to run in
    Cpl::System::Thread::create( scheduler_, "TSHELL" );

    // Start the shell
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting TShell...") );


    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Scheduling Enabled!") );

    // The test assumes that enableScheduling() never returns -->so make sure we don't return
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }
}
