#include "Cpl/System/Api.h"
#include "Catch/precompiled/main.h"

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // Run the test(s)
    return run_catch2_tests( argc, argv );
}
