/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/Text/strip.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "strip", "[strip]" )
{

	Shutdown_TS::clearAndUseCounter();

	SECTION( "stripSpace" )
	{
		const char* input = "  hello  world, \t bobs your uncle ";
		const char* ptr;

		ptr = stripSpace( input );
		REQUIRE( strcmp( ptr, "hello  world, \t bobs your uncle " ) == 0 );
		ptr = stripSpace( stripNotSpace( ptr ) );
		REQUIRE( strcmp( ptr, "world, \t bobs your uncle " ) == 0 );
		ptr = stripSpace( stripNotSpace( ptr ) );
		REQUIRE( strcmp( ptr, "bobs your uncle " ) == 0 );
		ptr = stripSpace( stripNotSpace( ptr ) );
		REQUIRE( strcmp( ptr, "your uncle " ) == 0 );
		ptr = stripSpace( stripNotSpace( ptr ) );
		REQUIRE( strcmp( ptr, "uncle " ) == 0 );
		ptr = stripSpace( stripNotSpace( ptr ) );
		REQUIRE( strcmp( ptr, "" ) == 0 );
	}

	SECTION( "stripChars" )
	{
		const char* input = "::hello,world,\t bobs your . uncle, ";
		const char* ptr;

		ptr = stripChars( input, ":.," );
		REQUIRE( strcmp( ptr, "hello,world,\t bobs your . uncle, " ) == 0 );
		ptr = stripChars( stripNotChars( ptr, ":.," ), ":.," );
		REQUIRE( strcmp( ptr, "world,\t bobs your . uncle, " ) == 0 );
		ptr = stripChars( stripNotChars( ptr, ":.," ), ":.," );
		REQUIRE( strcmp( ptr, "\t bobs your . uncle, " ) == 0 );
		ptr = stripChars( stripNotChars( ptr, ":.," ), ":.," );
		REQUIRE( strcmp( ptr, " uncle, " ) == 0 );
		ptr = stripChars( stripNotChars( ptr, ":.," ), ":.," );
		REQUIRE( strcmp( ptr, " " ) == 0 );
		ptr = stripChars( stripNotChars( ptr, ":.," ), ":.," );
		REQUIRE( strcmp( ptr, "" ) == 0 );
	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
