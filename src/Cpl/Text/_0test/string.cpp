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
#include "Cpl/Text/DString.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <fstream>

/// 
using namespace Cpl::Text;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "String", "[string]" )
{
	FString<10> foo( "Hello World, this is bob" );
	DString     bar( "Hello World, this is bob" );
	String&     fref = foo;
	String&     bref = bar;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Some basic operation" )
	{
		REQUIRE( foo != bar );
		REQUIRE( bar == bref );


		char c = bar;
		REQUIRE( c == 'H' );

		const char* ptr = foo;
		REQUIRE( strcmp( ptr, "Hello Worl" ) == 0 );

		ptr = bar();
		REQUIRE( strcmp( ptr, "Hello World, this is bob" ) == 0 );

		c = bref[3];
		REQUIRE( c == 'l' );
		c = fref[0];
		REQUIRE( c == 'H' );
		c = fref[10];
		REQUIRE( c == '\0' );
		c = bref[10];
		REQUIRE( c == 'd' );
		c = bref[-1];
		REQUIRE( c == '\0' );
		c = fref[9];
		REQUIRE( c == 'l' );
		c = bref[100];
		REQUIRE( c == '\0' );
	}

	SECTION( "Length..." )
	{
		REQUIRE( foo.length() == 10 );
		REQUIRE( foo.maxLength() == 10 );
		REQUIRE( foo.availLength() == 0 );
		REQUIRE( bar.length() == 24 );
		REQUIRE( bar.maxLength() == 31 );
		REQUIRE( bar.availLength() == 7 );
		REQUIRE( foo.truncated() == true );
		REQUIRE( bar.truncated() == false );
		REQUIRE( foo.isEmpty() == false );
		REQUIRE( bar.isEmpty() == false );
		DString s1;
		REQUIRE( s1.isEmpty() == true );
		foo.clear();
		REQUIRE( foo.isEmpty() == true );
		bar = "";
		REQUIRE( foo.isEmpty() == true );
		REQUIRE( foo.truncated() == false );
		REQUIRE( bar.truncated() == false );
	}

	SECTION( "Compare..." )
	{
		DString s1( "HELLO WORLD, THIS IS BOB" );
		const char* nullPtr = 0;

		REQUIRE( bar != s1 );
		REQUIRE( bar.isEqualIgnoreCase( s1 ) );
		REQUIRE( bar.isEqualIgnoreCase( "HELLO WORLD, THIS IS BOB" ) );
		REQUIRE( bar.isEqualIgnoreCase( 0 ) == false );
		REQUIRE( ( bar == nullPtr ) == false );
		REQUIRE( ( bar != nullPtr ) == true );

		DString s2 = 123;
		DString s3 = 124;
		DString s4 = 103;

		REQUIRE( s2.compare( s3 ) < 0 );
		REQUIRE( s3.compare( s2 ) > 0 );
		REQUIRE( s3.compare( s3 ) == 0 );
		REQUIRE( s4.compare( s2 ) < 0 );
		REQUIRE( s4.compare( nullPtr ) == -1 );

		REQUIRE( s1.compare( bar ) < 0 );
		REQUIRE( s1.compareIgnoreCase( bar ) == 0 );
		REQUIRE( s1.compareIgnoreCase( nullPtr ) == -1 );
		REQUIRE( bar.compare( s2 ) > 0 );

		REQUIRE( s1.isEqualSubstring( 0, 4, "hello" ) == false );
		REQUIRE( s1.isEqualSubstring( 6, 10, "world" ) == false );
		REQUIRE( s1.isEqualSubstring( 24, 28, "world" ) == false );
		REQUIRE( s1.isEqualSubstring( 24, 20, "world" ) == false );
		REQUIRE( s1.isEqualSubstring( 6, 10, "WORLD" ) == true );
		REQUIRE( s1.isEqualSubstring( -1, 4, "HELLO" ) == true );
		REQUIRE( s1.isEqualSubstring( -1, 4, nullPtr ) == false );
		REQUIRE( s1.isEqualSubstring( -1, 40, "HELLO WORLD, THIS IS BOB" ) == true );
		REQUIRE( s1.isEqualSubstring( -1, 40, "Hello World, this is bob" ) == false );
		REQUIRE( s1.isEqualSubstringIgnoreCase( 0, 4, "hello" ) == true );
		REQUIRE( s1.isEqualSubstringIgnoreCase( 6, 10, "world" ) == true );
		REQUIRE( s1.isEqualSubstringIgnoreCase( 6, 10, "WORLD" ) == true );
		REQUIRE( s1.isEqualSubstringIgnoreCase( 24, 1, "world" ) == false );
		REQUIRE( s1.isEqualSubstringIgnoreCase( 6, 10, nullPtr ) == false );
		REQUIRE( s1.isEqualSubstringIgnoreCase( -1, 4, "hello" ) == true );
		REQUIRE( s1.isEqualSubstringIgnoreCase( -1, 40, "HELLO WORLD, THIS IS BOB" ) == true );
		REQUIRE( s1.isEqualSubstringIgnoreCase( -1, 40, "Hello World, this is bob" ) == true );

		REQUIRE( s1.startsWith( "HELLO " ) == true );
		REQUIRE( s1.startsWith( "HELLo " ) == false );
		REQUIRE( s1.startsWith( "HELLO WORLD, THIS IS BOB" ) == true );
		REQUIRE( s1.startsWith( "HELLO WORLD  THIS IS BOB" ) == false );
		REQUIRE( s1.startsWith( "" ) == false );
		REQUIRE( s1.startsWith( nullPtr ) == false );
		REQUIRE( s1.startsWith( "WORLD", 6 ) == true );
		REQUIRE( s1.startsWith( "WORLD, THIS IS BOB", 6 ) == true );
		REQUIRE( s1.startsWith( nullPtr, 6 ) == false );
		REQUIRE( s1.startsWith( "WORLD, THIS IS BOB", 7 ) == false );
		REQUIRE( s1.startsWith( "WORLD, THIS IS BOB", -1 ) == false );
		REQUIRE( s1.startsWith( "WORLD, THIS IS BOB", 40 ) == false );
		REQUIRE( s1.startsWith( "HELLO", -1 ) == false );

		REQUIRE( s1.endsWith( "BOB" ) == true );
		REQUIRE( s1.endsWith( "BOb" ) == false );
		REQUIRE( s1.endsWith( "HELLO" ) == false );
		REQUIRE( s1.endsWith( nullPtr ) == false );
		REQUIRE( s1.endsWith( "" ) == false );

		REQUIRE( s1.indexOf( "HELLO WORLD, THIS IS BOB" ) == 0 );
		REQUIRE( s1.indexOf( "HELLO" ) == 0 );
		REQUIRE( s1.indexOf( "BOB" ) == 21 );
		REQUIRE( s1.indexOf( "THIS" ) == 13 );
		REQUIRE( s1.indexOf( "HELLOW" ) == -1 );
		REQUIRE( s1.indexOf( "WORLD" ) == 6 );
		REQUIRE( s1.indexOf( "WORLD", 0 ) == 6 );
		REQUIRE( s1.indexOf( "WORLD", 7 ) == -1 );
		REQUIRE( s1.indexOf( "WORLD", 6 ) == 6 );

		REQUIRE( s1.indexOf( "HELLO " ) == 0 );
		REQUIRE( s1.indexOf( "HELLo " ) == -1 );
		REQUIRE( s1.indexOf( "HELLO WORLD, THIS IS BOB" ) == 0 );
		REQUIRE( s1.indexOf( "HELLO WORLD  THIS IS BOB" ) == -1 );
		REQUIRE( s1.indexOf( "" ) == 0 );
		REQUIRE( s1.indexOf( nullPtr ) == -1 );
		REQUIRE( s1.indexOf( "WORLD", 6 ) == 6 );
		REQUIRE( s1.indexOf( "WORLD, THIS IS BOB", 6 ) == 6 );
		REQUIRE( s1.indexOf( nullPtr, 6 ) == -1 );
		REQUIRE( s1.indexOf( "WORLD, THIS IS BOB", 7 ) == -1 );
		REQUIRE( s1.indexOf( "WORLD, THIS IS BOB", -1 ) == -1 );
		REQUIRE( s1.indexOf( "WORLD, THIS IS BOB", 40 ) == -1 );
		REQUIRE( s1.indexOf( "HELLO", -1 ) == -1 );

		REQUIRE( s1.indexOf( "BOB" ) == 21 );
		REQUIRE( s1.indexOf( "BOb" ) == -1 );
		REQUIRE( s1.indexOf( " HELLO" ) == -1 );
	}

	SECTION( "Index..." )
	{
		DString s1( "Hello World, this is Bob" );
		const char* nullPtr = 0;

		REQUIRE( s1.indexOf( 'H' ) == 0 );
		REQUIRE( bar.indexOf( 'b' ) == 21 );
		REQUIRE( s1.indexOf( 'b' ) == 23 );
		REQUIRE( s1.indexOf( 'B' ) == 21 );
		REQUIRE( s1.indexOf( '.' ) == -1 );
		REQUIRE( s1.indexOf( 'o' ) == 4 );
		REQUIRE( s1.indexOf( 'o', 5 ) == 7 );
		REQUIRE( s1.indexOf( 'o', 4 ) == 4 );

		REQUIRE( s1.indexOf( "World" ) == 6 );
		REQUIRE( s1.indexOf( "world" ) == -1 );
		REQUIRE( s1.indexOf( "Hello" ) == 0 );
		REQUIRE( s1.indexOf( "b" ) == 23 );
		REQUIRE( s1.indexOf( "ob" ) == 22 );
		REQUIRE( s1.indexOf( "is", 17 ) == 18 );
		REQUIRE( s1.indexOf( "is", 33 ) == -1 );
		REQUIRE( s1.indexOf( "is", 16 ) == 18 );
		REQUIRE( s1.indexOf( "is", 15 ) == 15 );
		REQUIRE( s1.indexOf( "is", -1 ) == -1 );
		REQUIRE( s1.indexOf( nullPtr, 15 ) == -1 );

		REQUIRE( s1.lastIndexOf( 'b' ) == 23 );
		REQUIRE( s1.lastIndexOf( 'H' ) == 0 );
		REQUIRE( s1.lastIndexOf( 'o' ) == 22 );
		REQUIRE( s1.lastIndexOf( 'l' ) == 9 );
		REQUIRE( s1.lastIndexOf( 'X' ) == -1 );

		REQUIRE( s1.count( 'l' ) == 3 );
		REQUIRE( s1.count( 'x' ) == 0 );
		REQUIRE( s1.count( 'i' ) == 2 );
		REQUIRE( s1.count( 't' ) == 1 );
		REQUIRE( s1.count( 'b' ) == 1 );
		REQUIRE( s1.count( 'B' ) == 1 );
	}

	SECTION( "Printf..." )
	{
		const char* nullPtr = 0;

		foo.format( "%s=%X, was here", "Bob", -1 );
		bar.format( "%s=%X, was here", "Bob", -1 );

		REQUIRE( foo == "Bob=FFFFFF" );
		REQUIRE( bar == "Bob=FFFFFFFF, was here" );

		foo.formatAppend( ". %d", 12 );
		bar.formatAppend( ". %d", 12 );
		REQUIRE( foo == "Bob=FFFFFF" );
		REQUIRE( bar == "Bob=FFFFFFFF, was here. 12" );

		bar.formatAppend( "overflow the originally allocated block(s)" );
		REQUIRE( bar == "Bob=FFFFFFFF, was here. 12overf" );

		bar.format( nullPtr );
		REQUIRE( bar == "Bob=FFFFFFFF, was here. 12overf" );
		bar.format( "bob" );
		REQUIRE( bar == "bob" );
		bar.formatAppend( nullPtr );
		REQUIRE( bar == "bob" );
	}

	SECTION( "misc..." )
	{
		const char* nullPtr = 0;
		const char* ptr;

		REQUIRE( bar == "Hello World, this is bob" );
		ptr = bar.toUpper();
		REQUIRE( bar == "HELLO WORLD, THIS IS BOB" );
		REQUIRE( strcmp( ptr, "HELLO WORLD, THIS IS BOB" ) == 0 );

		REQUIRE( foo == "Hello Worl" );
		ptr = foo.toLower();
		REQUIRE( foo == "hello worl" );
		REQUIRE( strcmp( ptr, "hello worl" ) == 0 );

		DString s1 = " \tbobs your uncle  \n ";
		REQUIRE( s1 == " \tbobs your uncle  \n " );
		s1.removeLeadingSpaces();
		REQUIRE( s1 == "bobs your uncle  \n " );
		s1.removeLeadingSpaces();
		REQUIRE( s1 == "bobs your uncle  \n " );
		s1.removeTrailingSpaces();
		REQUIRE( s1 == "bobs your uncle" );
		s1.removeTrailingSpaces();
		REQUIRE( s1 == "bobs your uncle" );

		s1 = "   ";
		s1.removeLeadingSpaces();
		REQUIRE( s1 == "" );
		s1 = "   ";
		s1.removeTrailingSpaces();
		REQUIRE( s1 == "" );

		s1 = "xxz\tbobs your uncle  \n*@@*";
		s1.removeLeadingChars( nullPtr );
		s1.removeTrailingChars( nullPtr );
		REQUIRE( s1 == "xxz\tbobs your uncle  \n*@@*" );
		s1.removeLeadingChars( "xz" );
		REQUIRE( s1 == "\tbobs your uncle  \n*@@*" );
		s1.removeLeadingChars( "xz" );
		REQUIRE( s1 == "\tbobs your uncle  \n*@@*" );
		s1.removeTrailingChars( "*@" );
		REQUIRE( s1 == "\tbobs your uncle  \n" );
		s1.removeTrailingChars( "*@" );
		REQUIRE( s1 == "\tbobs your uncle  \n" );

		s1 = "zzxxxxz";
		s1.removeLeadingChars( "xz" );
		REQUIRE( s1 == "" );
		s1 = "*@####*@##";
		s1.removeTrailingChars( "#*@" );
		REQUIRE( s1 == "" );

		s1 = "123456789A";
		REQUIRE( s1 == "123456789A" );
		s1.cut( 0, 1 );
		REQUIRE( s1 == "3456789A" );
		s1.cut( 5, 3 );
		REQUIRE( s1 == "3456789A" );
		s1.cut( 5, 7 );
		REQUIRE( s1 == "34567" );
		s1.cut( 1, 2 );
		REQUIRE( s1 == "367" );
		s1.cut( -1, 2 );
		REQUIRE( s1 == "367" );
		s1.cut( 0, 3 );
		REQUIRE( s1 == "367" );

		s1 = "123456789A";
		REQUIRE( s1 == "123456789A" );
		s1.trimLeft( 3 );
		REQUIRE( s1 == "456789A" );
		s1.trimRight( 3 );
		REQUIRE( s1 == "4567" );
		s1.trimRight( -1 );
		REQUIRE( s1 == "4567" );
		s1.trimRight( 5 );
		REQUIRE( s1 == "4567" );
		s1.trimLeft( -1 );
		REQUIRE( s1 == "4567" );
		s1.trimLeft( 5 );
		REQUIRE( s1 == "4567" );
		s1.trimLeft( 1 );
		REQUIRE( s1 == "567" );
		s1.trimRight( 1 );
		REQUIRE( s1 == "56" );

		s1 = "1.2.3.4.5,6,7,8,9;a-b ";
		REQUIRE( s1 == "1.2.3.4.5,6,7,8,9;a-b " );
		s1.setChar( 0, '$' );
		REQUIRE( s1 == "$.2.3.4.5,6,7,8,9;a-b " );
		s1.setChar( 21, '#' );
		REQUIRE( s1 == "$.2.3.4.5,6,7,8,9;a-b#" );
		s1.setChar( -1, '#' );
		REQUIRE( s1 == "$.2.3.4.5,6,7,8,9;a-b#" );
		s1.setChar( 22, '#' );
		REQUIRE( s1 == "$.2.3.4.5,6,7,8,9;a-b#" );
		int count = s1.replace( '.', '+' );
		REQUIRE( s1 == "$+2+3+4+5,6,7,8,9;a-b#" );
		REQUIRE( count == 4 );
		count = s1.replace( 'A', '+' );
		REQUIRE( s1 == "$+2+3+4+5,6,7,8,9;a-b#" );
		REQUIRE( count == 0 );
		count = s1.replace( '8', '8' );
		REQUIRE( s1 == "$+2+3+4+5,6,7,8,9;a-b#" );
		REQUIRE( count == 0 );
		count = s1.replace( '3', 't' );
		REQUIRE( s1 == "$+2+t+4+5,6,7,8,9;a-b#" );
		REQUIRE( count == 1 );
	}

	SECTION( "iostream..." )
	{
		std::ofstream out( "stream.out", std::ios::trunc );
		DString o1( "Hello6789ABCDEF" );
		out << o1;
		DString o2( "World" );
		out << o2;
		out.close();

		std::ifstream in( "stream.out" );
		DString i1; // 16 bytes length
		in >> i1;
		REQUIRE( i1 == o1 );
		REQUIRE( i1.maxLength() == 15 );
		DString i2;
		in >> i2;
		REQUIRE( i2 == o2 );
		REQUIRE( i2.maxLength() == 15 );
		in.close();

	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}

