#include "Cpl/System/Api.h"

#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"


int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // Run the test(s)
    int result = Catch::Session().run( argc, argv );
    return result;
}
