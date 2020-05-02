#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/Container/Map.h"
#include "Cpl/TShell/Command.h"

// External references
extern void algorithmTest( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Io::Stdio::StdIn   infd_;
Cpl::Io::Stdio::StdOut  outfd_;


int main( int argc, char* const argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Run the test
    algorithmTest( infd_, outfd_ );

    // do not exit
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }

    return 0;
}
