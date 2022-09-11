#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/TShell/Cmd/Threads.h"


// External references
extern void shell_test4( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


Cpl::Container::Map<Cpl::TShell::Command>  cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
Cpl::TShell::Cmd::Threads                  threadsCmd_( cmdlist );
static Cpl::Io::Stdio::StdIn  infd_;
static Cpl::Io::Stdio::StdOut outfd_;

int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Run the test (the call never returns!)
    shell_test4( infd_, outfd_ );

    return 0;
}
