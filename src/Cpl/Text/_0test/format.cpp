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
#include "Cpl/Text/format.h"    
#include "Cpl/Text/DString.h"    
#include "Cpl/Text/FString.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <stdint.h>


/// 
using namespace Cpl::Text;
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "format", "[format]" )
{

	Shutdown_TS::clearAndUseCounter();

	SECTION( "bufferToSting...." )
	{
		uint8_t buffer[5] = { 0, 0xAA, 0x55, 0xF0, 0x31 };
		DString    s1;
		FString<3> s2;

		bool result = bufferToString( buffer, sizeof( buffer ), s1 );
		REQUIRE( s1 == "..U.1" );
		REQUIRE( result == true );

		s1 = "buffer=";
		bufferToString( buffer, sizeof( buffer ), s1, true );
		REQUIRE( s1 == "buffer=..U.1" );

		s1 = "no change";
		bufferToString( 0, sizeof( buffer ), s1 );
		REQUIRE( s1 == "no change" );
		bufferToString( buffer, 0, s1 );
		REQUIRE( s1 == "no change" );
		result = bufferToString( buffer, sizeof( buffer ), s2 );
		REQUIRE( result == false );
	}

	SECTION( "bufferToASCIIHex...." )
	{
		uint8_t buffer[5] = { 0, 0xAA, 0x55, 0xF0, 0x31 };
		DString    s1;
		FString<3> s2;

		bool result = bufferToAsciiHex( buffer, sizeof( buffer ), s1 );
		REQUIRE( s1 == "00AA55F031" );
		REQUIRE( result == true );
		bufferToAsciiHex( buffer, sizeof( buffer ), s1, false );
		REQUIRE( s1 == "00aa55f031" );


		s1 = "buffer=";
		bufferToAsciiHex( buffer, sizeof( buffer ), s1, true, true );
		REQUIRE( s1 == "buffer=00AA55F031" );

		s1 = "no change";
		bufferToAsciiHex( 0, sizeof( buffer ), s1 );
		REQUIRE( s1 == "no change" );
		bufferToAsciiHex( buffer, 0, s1 );
		REQUIRE( s1 == "no change" );
		result = bufferToAsciiHex( buffer, sizeof( buffer ), s2 );
		REQUIRE( result == false );
	}

	SECTION( "timestamp::sec...." )
	{
		DString    s1;
		FString<5> s2;

		bool result = formatSecTimeStamp( s1, 270183 );
		REQUIRE( result == true );
		REQUIRE( s1 == "03 03:03:03" );
		formatSecTimeStamp( s1, 270183, false );
		REQUIRE( s1 == "75:03:03" );

		result = formatSecTimeStamp( s2, 270183, false );
		REQUIRE( result == false );
	}

	SECTION( "timestamp::msec...." )
	{
		DString    s1;
		FString<5> s2;

		bool result = formatMsecTimeStamp( s1, 270183331 );
		REQUIRE( result == true );
		REQUIRE( s1 == "03 03:03:03.331" );
		formatMsecTimeStamp( s1, 270183331, false );
		REQUIRE( s1 == "75:03:03.331" );

		result = formatMsecTimeStamp( s2, 270183331, false );
		REQUIRE( result == false );
	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
