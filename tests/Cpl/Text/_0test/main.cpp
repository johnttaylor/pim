#include "Cpl/System/Api.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Memory/_testsupport/New_TS.h"
#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"



int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );
    CPL_SYSTEM_TRACE_FUNC( "_0test");

	// Expect EQUAL new/delete calls
	// Ignoring memory stats, Between the Catch framework (and Windoze) -->can't account for 'extra' news
    Cpl::Memory::New_TS::setNewDelete_delta( 0, true );


    // Run the test(s)
    int result = Catch::Session().run( argc, argv );

	// Shutdown Cpl so my testing infra-structure cleanup/post processing gets done
    Cpl::System::Shutdown_TS::restore();
    return Cpl::System::Shutdown::failure( result );
}
