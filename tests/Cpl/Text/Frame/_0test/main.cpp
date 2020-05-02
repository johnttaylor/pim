#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Catch/precompiled/main.h"

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();
    Cpl::System::Api::enableScheduling();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

    // Run the test(s)
    return run_catch2_tests( argc, argv );
}
