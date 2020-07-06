#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"


int main( int argc, char* argv[] )
{
	// Initialize Colony
	Cpl::System::Api::initialize();
	Cpl::System::Api::enableScheduling();

	CPL_SYSTEM_TRACE_ENABLE();
	CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
	CPL_SYSTEM_TRACE_ENABLE_SECTION( "Cpl::System::Shell" );
	//CPL_SYSTEM_TRACE_ENABLE_SECTION( "Cpl::System::Posix::Thread" );
	CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

	// Run the test(s)
    return Catch::Session().run( argc, argv );
}
