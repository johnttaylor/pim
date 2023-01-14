#include "../../Main.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/Io/Socket/Posix/Listener.h"
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

// Create a generic THREADS TShell command (cause I don't a POSIX specifc command yet..)
static Cpl::TShell::Cmd::Threads cmdThreads_( g_cmdlist );

// Create a socket listener
static Cpl::Io::Socket::Posix::Listener  listener;


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
