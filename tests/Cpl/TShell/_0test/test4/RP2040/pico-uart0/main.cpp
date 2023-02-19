#include "Bsp/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/System/RP2040/Console.h"

// External references
extern void shell_test4( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Container::Map<Cpl::TShell::Command>  cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
Cpl::TShell::Cmd::Threads                  threadsCmd_( cmdlist );

/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();

    // Run the test (the call never returns!)
    shell_test4( Cpl::System::RP2040::getConsoleStream(), Cpl::System::RP2040::getConsoleStream() );
    return 0;
}
