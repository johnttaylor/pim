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
#include "Cpl/Text/FString.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text;
/// 
using namespace Cpl::System;




////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "FString", "[fstring]" )
{
	FString<10> foo( "Hello World, this is bob" );
	FString<5>  bar( "Hello World, this is bob" );
	String&     bref = bar;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Some basic operation" )
	{
		REQUIRE( foo != bar );
		REQUIRE( bar == bref );

		/* 0123456789 */
		foo.insertAt( 0, "HI" );
		REQUIRE( foo == "HIHello Wo" );
		foo.insertAt( -1, "HI" );
		REQUIRE( foo == "HIHIHello " );
		foo.insertAt( 9, "HI" );
		REQUIRE( foo == "HIHIHelloH" );
		foo.insertAt( 10, "!!" );
		REQUIRE( foo == "HIHIHelloH" );
		foo.insertAt( 5, "<*>" );
		REQUIRE( foo == "HIHIH<*>el" );
		foo = "123";
		foo.insertAt( 10, "Bob's here" );
		REQUIRE( foo == "123Bob's h" );
		foo = "456";
		foo.insertAt( 1, "Uncle" );
		REQUIRE( foo == "4Uncle56" );
	}

	SECTION( "Constructors" )
	{
		const char* nullPtr = 0;
		FString<3> s0( nullPtr );
		REQUIRE( s0 == "" );

		FString<3> s1;
		REQUIRE( s1 == "" );

		s1 = "uncle";
		FString<3> s2( s1 );
		REQUIRE( s2 == "unc" );

		FString<3> s3( bref );
		REQUIRE( s3 == "Hel" );

		FString<3> s4( "abcdefghijklmnopqrstuvwxyz" );
		REQUIRE( s4 == "abc" );

		FString<3> s5( '@' );
		REQUIRE( s5 == '@' );

		FString<3> s6( -32 );
		REQUIRE( s6 == "-32" );

		FString<3> s7( (unsigned) 42 );
		REQUIRE( s7 == "42" );

		FString<3> s8( (long) -10000000 );
		REQUIRE( s8 == "-10" );

		FString<3> s9( (unsigned long) 81000000 );
		REQUIRE( s9 == "810" );

		FString<3> s10( (long long) -200000000000 );
		REQUIRE( s10 == "-20" );

		FString<3> s11( ( unsigned long long )9100000000000 );
		REQUIRE( s11 == "910" );

		FString<0> s12( "hello" );
		REQUIRE( s12 == "" );
		FString<0> s13( '#' );
		REQUIRE( s13 == "" );
	}

	SECTION( "Assignments" )
	{
		const char* nullPtr = 0;
		FString<3> s0( "****" );
		s0 = nullPtr;
		REQUIRE( s0 == "" );

		FString<3> s1( "****" );
		s1 = "";
		REQUIRE( s1 == "" );

		s1 = "uncle";
		FString<3> s2( "****" );
		s2 = s1;
		REQUIRE( s2 == "unc" );

		FString<3> s3( "****" );
		s3 = bref;
		REQUIRE( s3 == "Hel" );

		FString<3> s4( "****" );
		s4 = "abcdefghijklmnopqrstuvwxyz";
		REQUIRE( s4 == "abc" );

		FString<3> s5( "****" );
		s5 = '@';
		REQUIRE( s5 == '@' );

		FString<3> s6( "****" );
		s6 = -32;
		REQUIRE( s6 == "-32" );

		FString<3> s7( "****" );
		s7 = (unsigned) 42;;
		REQUIRE( s7 == "42" );

		FString<3> s8( "****" );
		s8 = (long) -10000000;
		REQUIRE( s8 == "-10" );

		FString<3> s9( "****" );
		s9 = (unsigned long) 81000000;
		REQUIRE( s9 == "810" );

		FString<3> s10( "****" );
		s10 = (long long) -200000000000;
		REQUIRE( s10 == "-20" );

		FString<3> s11( "****" );
		s11 = ( unsigned long long ) 9100000000000;
		REQUIRE( s11 == "910" );
	}

	SECTION( "Append" )
	{
		const char* nullPtr = 0;
		FString<8> s0( "****" );
		s0 += nullPtr;
		REQUIRE( s0 == "****" );

		FString<8> s1( "****" );
		s1 += "";
		REQUIRE( s1 == "****" );

		s1 = "uncle";
		FString<8> s2( "****" );
		s2 += s1;
		REQUIRE( s2 == "****uncl" );

		FString<8> s3( "****" );
		s3 += bref;
		REQUIRE( s3 == "****Hell" );

		FString<8> s4( "****" );
		s4 += "abcdefghijklmnopqrstuvwxyz";
		REQUIRE( s4 == "****abcd" );

		FString<8> s5( "****" );
		s5 += '@';
		REQUIRE( s5 == "****@" );

		FString<8> s6( "****" );
		s6 += -32;
		REQUIRE( s6 == "****-32" );

		FString<8> s7( "****" );
		s7 += (unsigned) 42;;
		REQUIRE( s7 == "****42" );

		FString<8> s8( "****" );
		s8 += (long) -10000000;
		REQUIRE( s8 == "****-100" );

		FString<8> s9( "****" );
		s9 += (unsigned long) 81000000;
		REQUIRE( s9 == "****8100" );

		FString<8> s10( "****" );
		s10 += (long long) -200000000000;
		REQUIRE( s10 == "****-200" );

		FString<8> s11( "****" );
		s11 += ( unsigned long long ) 9100000000000;
		REQUIRE( s11 == "****9100" );
	}
	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
