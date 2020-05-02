#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Socket/Posix/Listener.h"
#include "Cpl/Io/Socket/Posix/Connector.h"
#include "Cpl/Io/Socket/_0test/loopback.h"
#include "Catch/precompiled/main.h"




int main( int argc, char* argv[] )
{
	// Initialize Colony
	Cpl::System::Api::initialize();
	Cpl::System::Api::enableScheduling();

	// Platform specific listener
	Cpl::Io::Socket::Posix::Listener  listener;
	Cpl::Io::Socket::Posix::Connector connector;
	initialize_loopback( listener, connector );

	CPL_SYSTEM_TRACE_ENABLE();
	CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
	CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

	// Run the test(s)
	return run_catch2_tests( argc, argv );
}

