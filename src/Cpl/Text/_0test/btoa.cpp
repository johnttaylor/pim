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
#include "Cpl/Text/btoa.h"    
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <stdint.h>
#include "colony_map.h"

#define SECT_ "_0test"


/// 
using namespace Cpl::Text;
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "btoa", "[btoa]" )
{

	Shutdown_TS::clearAndUseCounter();

	SECTION( "long...." )
	{
		char        buffer[sizeof( long ) / 4 * 10 + 1];
		const char* leftPtr;
		long        value;

		value   = 0x32;
		leftPtr = longToStr( value, buffer, 4 + 1, 10 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "50" ) == 0 );
		REQUIRE( strcmp( buffer, "  50" ) == 0 );

		leftPtr = longToStr( value, buffer, 4 + 1, 10, '0' );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "50" ) == 0 );
		REQUIRE( strcmp( buffer, "0050" ) == 0 );

		leftPtr = longToStr( value, buffer, 4 + 1, 16, '0' );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%lX, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "32" ) == 0 );
		REQUIRE( strcmp( buffer, "0032" ) == 0 );

		leftPtr = longToStr( value, buffer, 6 + 1, 2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "110010" ) == 0 );
		REQUIRE( strcmp( buffer, "110010" ) == 0 );

		leftPtr = longToStr( value, buffer, 4 + 1, 8 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "62" ) == 0 );
		REQUIRE( strcmp( buffer, "  62" ) == 0 );

		value   = -50;
		leftPtr = longToStr( value, buffer, 4 + 1, 10 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "-50" ) == 0 );
		REQUIRE( strcmp( buffer, " -50" ) == 0 );

		leftPtr = longToStr( value, buffer, 4 + 1, 10, '0' );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "-50" ) == 0 );
		REQUIRE( strcmp( buffer, "0-50" ) == 0 );

		leftPtr = longToStr( value, buffer, 4 + 1, 16, '0' );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "-32" ) == 0 );
		REQUIRE( strcmp( buffer, "0-32" ) == 0 );

		leftPtr = longToStr( value, buffer, 6 + 1, 2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "-10010" ) == 0 );
		REQUIRE( strcmp( buffer, "-10010" ) == 0 );

		leftPtr = longToStr( value, buffer, 8 + 1, 2 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%ld, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "-110010" ) == 0 );
		REQUIRE( strcmp( buffer, " -110010" ) == 0 );
	}

	SECTION( "unsigned long..." )
	{
		char        buffer[sizeof( unsigned long ) / 4 * 10 + 1];
		const char* leftPtr;
		long        value;

		value   = 0x3B;
		leftPtr = ulongToStr( value, buffer, 4 + 1, 10 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%lu, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "59" ) == 0 );
		REQUIRE( strcmp( buffer, "  59" ) == 0 );

		leftPtr = ulongToStr( value, buffer, 4 + 1, 10, '0' );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=%lu, rstr=[%s], lstr=[%s]", value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "59" ) == 0 );
		REQUIRE( strcmp( buffer, "0059" ) == 0 );
	}

	SECTION( "size_t..." )
	{
		char        buffer[sizeof( unsigned long ) / 4 * 10 + 1];
		const char* leftPtr;
		long        value;

		value   = 0x32;
		leftPtr = sizetToStr( value, buffer, 4 + 1, 10 );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=0x%p, rstr=[%s], lstr=[%s]", (void*) value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "50" ) == 0 );
		REQUIRE( strcmp( buffer, "  50" ) == 0 );

		leftPtr = sizetToStr( value, buffer, 4 + 1, 10, '0' );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "value=0x%p, rstr=[%s], lstr=[%s]", (void*) value, buffer, leftPtr ) );
		REQUIRE( strcmp( leftPtr, "50" ) == 0 );
		REQUIRE( strcmp( buffer, "0050" ) == 0 );
	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
