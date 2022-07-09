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
#include "Cpl/Text/DString.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Memory/_testsupport/New_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DString", "[dstring]" )
{
	DString foo( "Hello World, this is bob" );
	DString bar( "Hello World, this is bob" );
	String& bref = bar;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Some basic operation" )
	{
		REQUIRE( foo == bar );
		REQUIRE( bar == bref );

		/* 0123456789 */
		foo.insertAt( 0, "HI" );
		REQUIRE( foo == "HIHello World, this is bob" );
		foo.insertAt( -1, "HI" );
		REQUIRE( foo == "HIHIHello World, this is bob" );
		foo.insertAt( 9, "HI" );
		REQUIRE( foo == "HIHIHelloHI World, this is bob" );
		foo.insertAt( 10, "!!" );
		REQUIRE( foo == "HIHIHelloH!!I World, this is bob" );
		foo.insertAt( 5, "<*>" );
		REQUIRE( foo == "HIHIH<*>elloH!!I World, this is bob" );
		foo = "123";
		foo.insertAt( 10, "Bob's here" );
		REQUIRE( foo == "123Bob's here" );
		foo = "456";
		foo.insertAt( 1, "Uncle" );
		REQUIRE( foo == "4Uncle56" );

		DString s( "123456789 123456" );
		REQUIRE( s == "123456789 123456" );
	}

	SECTION( "Constructors" )
	{
		const char* nullPtr = 0;
		DString s0( nullPtr );
		REQUIRE( s0 == "" );

		DString s1;
		REQUIRE( s1 == "" );

		s1 = "uncle";
		DString s2( s1 );
		REQUIRE( s2 == "uncle" );

		DString s3( bref );
		REQUIRE( s3 == "Hello World, this is bob" );

		DString s4( "abcdefghijklmnopqrstuvwxyz" );
		REQUIRE( s4 == "abcdefghijklmnopqrstuvwxyz" );

		DString s5( '@' );
		REQUIRE( s5 == '@' );

		DString s6( -32 );
		REQUIRE( s6 == "-32" );

		DString s7( (unsigned) 42 );
		REQUIRE( s7 == "42" );

		DString s8( (long) -10000000 );
		REQUIRE( s8 == "-10000000" );

		DString s9( (unsigned long) 81000000 );
		REQUIRE( s9 == "81000000" );

		DString s10( (long long) -200000000000 );
		REQUIRE( s10 == "-200000000000" );

		DString s11( ( unsigned long long )9100000000000 );
		REQUIRE( s11 == "9100000000000" );
	}

	SECTION( "Assignments" )
	{
		const char* nullPtr = 0;
		DString s0( "****" );
		s0 = nullPtr;
		REQUIRE( s0 == "" );

		DString s1( "****" );
		s1 = "";
		REQUIRE( s1 == "" );

		s1 = "uncle";
		DString s2( "****" );
		s2 = s1;
		REQUIRE( s2 == "uncle" );

		DString s3( "****" );
		s3 = bref;
		REQUIRE( s3 == "Hello World, this is bob" );

		DString s4( "****" );
		s4 = "abcdefghijklmnopqrstuvwxyz";
		REQUIRE( s4 == "abcdefghijklmnopqrstuvwxyz" );

		DString s5( "****" );
		s5 = '@';
		REQUIRE( s5 == '@' );

		DString s6( "****" );
		s6 = -32;
		REQUIRE( s6 == "-32" );

		DString s7( "****" );
		s7 = (unsigned) 42;;
		REQUIRE( s7 == "42" );

		DString s8( "****" );
		s8 = (long) -10000000;
		REQUIRE( s8 == "-10000000" );

		DString s9( "****" );
		s9 = (unsigned long) 81000000;
		REQUIRE( s9 == "81000000" );

		DString s10( "****" );
		s10 = (long long) -200000000000;
		REQUIRE( s10 == "-200000000000" );

		DString s11( "****" );
		s11 = ( unsigned long long ) 9100000000000;
		REQUIRE( s11 == "9100000000000" );
	}

	SECTION( "Append" )
	{
		const char* nullPtr = 0;
		DString s0( "****" );
		s0 += nullPtr;
		REQUIRE( s0 == "****" );

		DString s1( "****" );
		s1 += "";
		REQUIRE( s1 == "****" );

		s1 = "uncle";
		DString s2( "****" );
		s2 += s1;
		REQUIRE( s2 == "****uncle" );

		DString s3( "****" );
		s3 += bref;
		REQUIRE( s3 == "****Hello World, this is bob" );

		DString s4( "****" );
		s4 += "abcdefghijklmnopqrstuvwxyz";
		REQUIRE( s4 == "****abcdefghijklmnopqrstuvwxyz" );

		DString s5( "****" );
		s5 += '@';
		REQUIRE( s5 == "****@" );

		DString s6( "****" );
		s6 += -32;
		REQUIRE( s6 == "****-32" );

		DString s7( "****" );
		s7 += (unsigned) 42;;
		REQUIRE( s7 == "****42" );

		DString s8( "****" );
		s8 += (long) -10000000;
		REQUIRE( s8 == "****-10000000" );

		DString s9( "****" );
		s9 += (unsigned long) 81000000;
		REQUIRE( s9 == "****81000000" );

		DString s10( "****" );
		s10 += (long long) -200000000000;
		REQUIRE( s10 == "****-200000000000" );

		DString s11( "****" );
		s11 += ( unsigned long long ) 9100000000000;
		REQUIRE( s11 == "****9100000000000" );
	}

	SECTION( "Memory..." )
	{
		DString s1( "unclexx" );
		Cpl::Memory::New_TS::disable();
		DString s2( s1 );
		DString s3( bref );
		DString s4( "abcdefghijklmnopqrstuvwxyz" );
		DString s5( '%' );
		DString s6( -32 );
		DString s7( (unsigned) 42 );
		DString s8( (long) -10000000 );
		DString s9( (unsigned long) 81000000 );
		DString s10( (long long) -81000000 );
		DString s11( ( unsigned long long )9100000000000 );
		Cpl::Memory::New_TS::enable();
		REQUIRE( s1 == ( const char* )"unclexx" );
		REQUIRE( s2 == "" );
		REQUIRE( s2.length() == 0 );
		REQUIRE( s3 == "" );
		REQUIRE( s3.length() == 0 );
		REQUIRE( s4 == "" );
		REQUIRE( s4.length() == 0 );
		REQUIRE( s5 == "" );
		REQUIRE( s5.length() == 0 );
		REQUIRE( s6 == "" );
		REQUIRE( s6.length() == 0 );
		REQUIRE( s7 == "" );
		REQUIRE( s7.length() == 0 );
		REQUIRE( s8 == "" );
		REQUIRE( s8.length() == 0 );
		REQUIRE( s9 == "" );
		REQUIRE( s9.length() == 0 );
		REQUIRE( s10 == "" );
		REQUIRE( s10.length() == 0 );
		REQUIRE( s11 == "" );
		REQUIRE( s11.length() == 0 );


		DString s12( "Hi" );
		Cpl::Memory::New_TS::disable();
		s12 += " World";
		s12+= "overflow the block size on the append operation!!!!!! add a few more characters just to max sure";
		Cpl::Memory::New_TS::enable();
		REQUIRE( s12.length() == 15 );
		REQUIRE( s12 == "Hi Worldoverflo" );

		s12 = "bob";
		REQUIRE( s12 == "bob" );
		Cpl::Memory::New_TS::disable();
		s12 = "overflow the block size on the append operation!!!!!! add a few more characters just to max sure";
		Cpl::Memory::New_TS::enable();
		REQUIRE( s12.length() == 15 );
		REQUIRE( s12 == "overflow the bl" );

		Cpl::Memory::New_TS::disable();
		s12.insertAt( 5, "[hello]" );
		Cpl::Memory::New_TS::enable();
		REQUIRE( s12 == "overf[hello]low" );

		Cpl::Memory::New_TS::disable();
		s2.insertAt( 5, "[hello]" );
		s3.insertAt( 1, "" );
		Cpl::Memory::New_TS::enable();
		REQUIRE( s2 == "" );
		REQUIRE( s2.length() == 0 );
		REQUIRE( s2.maxLength() == 0 );
		REQUIRE( s3 == "" );
		REQUIRE( s3.length() == 0 );
		REQUIRE( s3.maxLength() == 0 );
	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
