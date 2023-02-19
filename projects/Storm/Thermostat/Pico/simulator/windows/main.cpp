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

#include "Storm/Thermostat/Main/Main.h"
#include "Cpl/Io/Socket/Win32/Connector.h"
#include "Cpl/Io/Socket/InputOutput.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/InputOutput.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Driver/PicoDisplay/TPipe/Api.h"
#include "docopt-cpp/docopt.h"
#include "docopt-cpp/docopt_value.h"

static const char USAGE[] =
R"(Storm Thermostat Simulation.

    Usage:
      thermostat-sim [options]

    Options:
      -h HOST       Hostname for the Display Simulation. [Default: 127.0.0.1]
      -p PORT       The Display Simulation's Port number [Default: 5010]

      -v            Be verbose
      --help        Show this screen.

)";


#define SIMULATION_TITLE        "Storm Thermostat"
#define TITLE_COMMAND           "00 00:00:00.000 title " SIMULATION_TITLE


static std::map<std::string, docopt::value> args_;

/// Create the console streams 
static Cpl::Io::Stdio::StdIn  consoleInfd_;
static Cpl::Io::Stdio::StdOut consoleOutfd_;

static Cpl::TShell::Cmd::Win32::Threads cmdThreads_( g_cmdlist );

////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
    // Parse the command line arguments
    args_ = docopt::docopt( USAGE, { argv + 1, argv + argc }, true );

    // Initialize Colony C++ library
    Cpl::System::Api::initialize();

    // Platform specific sockets: Connect to the Display simulation
    Cpl::Io::Descriptor                 socketFd;
    Cpl::Io::Socket::Win32::Connector   connector;
    if ( connector.establish( args_["-h"].asString().c_str(), args_["-p"].asLong(), socketFd ) != Cpl::Io::Socket::Connector::eSUCCESS )
    {
        printf( "\nERROR. Failed to connected to the Display Simulation. (host=%s, port=%d).\n",
                args_["-h"].asString().c_str(), 
                args_["-p"].asLong() );
        return 1;
    }
    Cpl::Io::Socket::InputOutput socketStream( socketFd );

    // Initializes the simulator's socket connect to the GUI application
    Driver::PicoDisplay::TPipe::initialize( socketStream, socketStream );

    // Set the title for the Simulation window
    Cpl::System::Api::sleep( 50 ); // Allow time for the TPipe thread to spin up
    Driver::PicoDisplay::TPipe::tpipe().sendCommand( TITLE_COMMAND, strlen( TITLE_COMMAND ) );

    // Launch the application (on the simulation platform this method returns -->but don't the executable end)
    runTheApplication( consoleInfd_, consoleOutfd_ );
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000000 );
    }

    // If I get here something BAD happen!
    return 1;
}
