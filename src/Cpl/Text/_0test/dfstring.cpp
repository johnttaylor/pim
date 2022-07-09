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
#include "Cpl/Text/DFString.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Memory/_testsupport/New_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text;
using namespace Cpl::System;




////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DFString", "[dfstring]" )
{
	DFString foo( 10, "Hello World, this is bob" );
	DFString bar( 5, "Hello World, this is bob" );
	String&     fref = foo;
	String&     bref = bar;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Some basic operation" )
	{
		REQUIRE( foo == bar );
		REQUIRE( bar == bref );

		/* 0123456789 1235*/
		foo.insertAt( 0, "HI" );
		REQUIRE( foo == "HIHello World, " );
		foo.insertAt( -1, "HI" );
		REQUIRE( foo == "HIHIHello World" );
		foo.insertAt( 9, "HI" );
		REQUIRE( foo == "HIHIHelloHI Wor" );
		foo.insertAt( 10, "!!" );
		REQUIRE( foo == "HIHIHelloH!!I W" );
		foo.insertAt( 5, "<*>" );
		REQUIRE( foo == "HIHIH<*>elloH!!" );
		foo = "123";
		foo.insertAt( 10, "Bob's here" );
		REQUIRE( foo == "123Bob's here" );
		foo = "456";
		foo.insertAt( 1, "Uncle" );
		REQUIRE( foo == "4Uncle56" );

		DFString s1( 3, foo );
		REQUIRE( s1 == "4Uncle56" );
		DFString s2( 6, bref );
		REQUIRE( s2 == "Hello World, th" );
		DFString s3( 0, fref );
		REQUIRE( s3 == "4Uncle56" );
	}

	SECTION( "Constructors" )
	{
		const char* nullPtr = 0;
		DFString s0( 3, nullPtr );
		REQUIRE( s0 == "" );

		DFString s00( 0, "Hello World, this is bob" );
		REQUIRE( s00 == "Hello World, th" );

		DFString s1;
		REQUIRE( s1 == "" );

		s1 = "uncle";
		DFString s2( s1 );
		REQUIRE( s2 == ( const char* )"uncle" );

		DFString s3( bref );
		REQUIRE( s3 == "Hello World, th" );

		DFString s4( 3, "abcdefghijklmnopqrstuvwxyz" );
		REQUIRE( s4 == "abcdefghijklmno" );

		DFString s5( 3, '@' );
		REQUIRE( s5 == '@' );

		DFString s6( 3, -32 );
		REQUIRE( s6 == "-32" );

		DFString s7( 3, (unsigned) 42 );
		REQUIRE( s7 == "42" );

		DFString s8( 3, (long) -10000000 );
		REQUIRE( s8 == "-10000000" );

		DFString s9( 3, (unsigned long) 81000000 );
		REQUIRE( s9 == "81000000" );

		DFString s10( 3, (long long) -81000000 );
		REQUIRE( s10 == "-81000000" );

		DFString s11( 3, ( unsigned long long )9100000000000 );
		REQUIRE( s11 == "9100000000000" );
	}

	SECTION( "Assignments" )
	{
		const char* nullPtr = 0;
		DFString s0( 3, "****" );
		s0 = nullPtr;
		REQUIRE( s0 == "" );

		DFString s1( 3, "****" );
		s1 = "";
		REQUIRE( s1 == "" );

		s1 = "uncle";
		DFString s2( 3, "****" );
		s2 = s1;
		REQUIRE( s2 == "uncle" );

		DFString s3( 3, "****" );
		s3 = bref;
		REQUIRE( s3 == "Hello World, th" );

		DFString s4( 3, "****" );
		s4 = "abcdefghijklmnopqrstuvwxyz";
		REQUIRE( s4 == "abcdefghijklmno" );

		DFString s5( 3, "****" );
		s5 = '@';
		REQUIRE( s5 == '@' );

		DFString s6( 3, "****" );
		s6 = -32;
		REQUIRE( s6 == "-32" );

		DFString s7( 3, "****" );
		s7 = (unsigned) 42;;
		REQUIRE( s7 == "42" );

		DFString s8( 3, "****" );
		s8 = (long) -10000000;
		REQUIRE( s8 == "-10000000" );

		DFString s9( 3, "****" );
		s9 = (unsigned long) 81000000;
		REQUIRE( s9 == "81000000" );

		DFString s10( 3, "****" );
		s10 = (long long) -200000000000;
		REQUIRE( s10 == "-200000000000" );

		DFString s11( 3, "****" );
		s11 = ( unsigned long long ) 9100000000000;
		REQUIRE( s11 == "9100000000000" );
	}

	SECTION( "Append" )
	{
		const char* nullPtr = 0;
		DFString s0( 8, "****" );
		s0 += nullPtr;
		REQUIRE( s0 == "****" );

		DFString s1( 8, "****" );
		s1 += "";
		REQUIRE( s1 == "****" );

		s1 = "uncle";
		DFString s2( 8, "****" );
		s2 += s1;
		REQUIRE( s2 == "****uncle" );

		DFString s3( 8, "****" );
		s3 += bref;
		REQUIRE( s3 == "****Hello World" );

		DFString s4( 8, "****" );
		s4 += "abcdefghijklmnopqrstuvwxyz";
		REQUIRE( s4 == "****abcdefghijk" );

		DFString s5( 8, "****" );
		s5 += '@';
		REQUIRE( s5 == "****@" );

		DFString s6( 8, "****" );
		s6 += -32;
		REQUIRE( s6 == "****-32" );

		DFString s7( 8, "****" );
		s7 += (unsigned) 42;;
		REQUIRE( s7 == "****42" );

		DFString s8( 8, "****" );
		s8 += (long) -10000000;
		REQUIRE( s8 == "****-10000000" );

		DFString s9( 8, "****" );
		s9 += (unsigned long) 81000000;
		REQUIRE( s9 == "****81000000" );

		DFString s10( 8, "****" );
		s10 += (long long) -200000000000;
		REQUIRE( s10 == "****-2000000000" );

		DFString s11( 8, "****" );
		s11 += ( unsigned long long ) 9100000000000;
		REQUIRE( s11 == "****91000000000" );
	}

	SECTION( "Memory..." )
	{
		DFString s1( 10, "uncle" );
		Cpl::Memory::New_TS::disable();
		DFString s2( s1 );
		DFString s3( bref );
		DFString s4( 3, "abcdefghijklmnopqrstuvwxyz" );
		DFString s5( 3, '@' );
		DFString s6( 3, -32 );
		DFString s7( 3, (unsigned) 42 );
		DFString s8( 3, (long) -10000000 );
		DFString s9( 3, (unsigned long) 81000000 );
		DFString s10( 3, (long long) -81000000 );
		DFString s11( 3, ( unsigned long long )9100000000000 );
		Cpl::Memory::New_TS::enable();
		REQUIRE( s1 == ( const char* )"uncle" );
		REQUIRE( s2.length() == 0 );
		REQUIRE( s2 == "" );
		REQUIRE( s3.length() == 0 );
		REQUIRE( s3 == "" );
		REQUIRE( s4.length() == 0 );
		REQUIRE( s4 == "" );
		REQUIRE( s5.length() == 0 );
		REQUIRE( s5 == "" );
		REQUIRE( s6.length() == 0 );
		REQUIRE( s6 == "" );
		REQUIRE( s7.length() == 0 );
		REQUIRE( s7 == "" );
		REQUIRE( s8.length() == 0 );
		REQUIRE( s8 == "" );
		REQUIRE( s9.length() == 0 );
		REQUIRE( s9 == "" );
		REQUIRE( s10.length() == 0 );
		REQUIRE( s10 == "" );
		REQUIRE( s11.length() == 0 );
		REQUIRE( s11 == "" );

		Cpl::Memory::New_TS::disable();
		s2 += "hello";
		Cpl::Memory::New_TS::enable();
		REQUIRE( s2.length() == 0 );
		REQUIRE( s2 == "" );
	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
