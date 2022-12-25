/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/Json/ObjectDetector.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"


#define SECT_   "_0test"

/// 
using namespace Cpl::Json;



////////////////////////////////////////////////////////////////////////////////

static bool scan( ObjectDetector& detector, const char* stringInput, size_t chunkSize, size_t& startOffset, size_t& endOffset )
{
	size_t      inLen = strlen( stringInput );
	const char* ptr   = stringInput;
	while ( *stringInput != '\0' && inLen > 0 )
	{
		// Don't read past the end of the input
		if ( chunkSize > inLen )
		{
			chunkSize = inLen;
		}

		if ( detector.scan( ptr, chunkSize, startOffset, endOffset ) )
		{
			return true;
		}
		ptr   += chunkSize;
		inLen -= chunkSize;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "ObjectDetector" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	size_t			startOffset;
	size_t			endOffset;
	ObjectDetector	uut;

	REQUIRE( scan( uut, "{}", 10, startOffset, endOffset ) );
	REQUIRE( startOffset == 0 );
	REQUIRE( endOffset == 1 );

	REQUIRE( scan( uut, "{}", 10, startOffset, endOffset ) );
	REQUIRE( startOffset == 0 );
	REQUIRE( endOffset == 0 );
	
	uut.reset();
	REQUIRE( scan( uut, "{bob:\"hello\",}", 3, startOffset, endOffset ) );
	REQUIRE( startOffset == 0 );
	REQUIRE( endOffset == 13 );

	uut.reset();
	REQUIRE( scan( uut, "{bob:\"he}llo\",}", 4, startOffset, endOffset ) );
	REQUIRE( startOffset == 0 );
	REQUIRE( endOffset == 14 );

	uut.reset();
	REQUIRE( scan( uut, "{bob:\"he\\\"}llo\",}", 1, startOffset, endOffset ) );
	REQUIRE( startOffset == 0 );
	REQUIRE( endOffset == 16 );

	uut.reset();
	REQUIRE( scan( uut, "{bob:\"he\\\"}llo\",", 10, startOffset, endOffset ) == false );

	//
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
