#include "Cpl/Io/Socket/Win32/Listener.h"
#include "Cpl/Io/Socket/Win32/Connector.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Socket/_0test/loopback.h"
#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"



int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();
    Cpl::System::Api::enableScheduling();

    // Platform specific listener
    Cpl::Io::Socket::Win32::Listener  listener;
    Cpl::Io::Socket::Win32::Connector connector;
    initialize_loopback( listener, connector );

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

    // Run the test(s)
    return Catch::Session().run( argc, argv );
}
