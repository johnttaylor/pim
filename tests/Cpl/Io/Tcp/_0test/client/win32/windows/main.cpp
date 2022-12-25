#include "Cpl/Io/Tcp/Win32/AsyncConnector.h"
#include "colony_config.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Tcp/_0test/client.h"

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // Platform specific listener
    Cpl::Io::Tcp::Win32::AsyncConnector  connector;

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "*Cpl::Io::Tcp" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Run the test(s)
    return runTest( connector, OPTION_REMOTE_HOST_ADDRESS, OPTION_TCP_TEST_PORT_NUM );
}
