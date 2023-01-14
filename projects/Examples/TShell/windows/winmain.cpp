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

#include "Cpl/Io/Socket/Win32/Listener.h" // Note: This include MUST be first because of Windoze's handling of 'winsock2.h'
#include "../../Main.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Win32/Threads.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "docopt-cpp/docopt.h"

static const char USAGE[] =
R"(TShell Example.

    Usage:
      tshell-example [options]

    Options:
      -n            Create and run a non-blocking TShell that uses STDIO for as 
                    the IO streams.
      -s PORT       Create and run a blocking TShell that uses TCP sockets as
                    the IO streams. The TShell is socket listener on port 
                    number: PORT
      -h,--help     Show this screen.

    NOTES:
     o The default behavior is to create and run a blocking TShell that uses 
       STDIO as the IO streams.

)";

// Create a platform specific THREADS TShell command
static Cpl::TShell::Cmd::Win32::Threads cmdThreads_( g_cmdlist );

// Create a socket listener
static Cpl::Io::Socket::Win32::Listener  listener;

// Create IO Streams for STDIO
Cpl::Io::Stdio::StdIn   infd_;
Cpl::Io::Stdio::StdOut  outfd_;

std::map<std::string, docopt::value>    args_;

int main( int argc, char* const argv[] )
{
    MyShellOption_T shellOption = SHELL_BLOCKING;
    int             portNum     = 5002;

    // Process Command line options
    args_ = docopt::docopt( USAGE, { argv + 1, argv + argc }, true );
    if ( args_["-n"].asBool() )
    {
        shellOption = SHELL_NON_BLOCKING;
    }
    else if ( args_["-s"] )
    {
        shellOption = SHELL_SOCKETS;
        portNum     = args_["-s"].asLong();
    }

    // Initialize Colony
    Cpl::System::Api::initialize();

    // Run the application
    return runTheApplication( shellOption, infd_, outfd_, listener, portNum );
}
