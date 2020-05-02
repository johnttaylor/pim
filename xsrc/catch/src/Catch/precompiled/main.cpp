#include "main.h"
#define CATCH_CONFIG_RUNNER  
#include "Catch/catch.hpp"

int run_catch2_tests( int argc, char* argv[] )
{
	return Catch::Session().run( argc, argv );
}