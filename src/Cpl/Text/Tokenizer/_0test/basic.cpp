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
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/Basic.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text::Tokenizer;


#define SECT_   "_0test"

////////////////////////////////////////////////////////////////////////////////


#define DELIMITERS_     ",.()"



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "basic", "[basic]" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	SECTION( "Whitespace#1..." )
	{
		int                    dummy;
		Cpl::Text::FString<64> token;
		Cpl::Text::FString<64> rawString = "bob's  your \n uncle";
		Basic                  parser( rawString.getBuffer( dummy ) );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		REQUIRE( parser.numTokens() == 1 );
		REQUIRE( parser.getToken( 1 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == " your \n uncle" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "your" );
		REQUIRE( parser.numTokens() == 2 );
		REQUIRE( parser.getToken( 2 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.getToken( 1 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "your" );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "\n uncle" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "uncle" );
		REQUIRE( parser.numTokens() == 3 );
		REQUIRE( parser.getToken( 3 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.getToken( 1 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "your" );
		token = parser.getToken( 2 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "uncle" );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() );
		REQUIRE( parser.numTokens() == 3 );
		REQUIRE( parser.getToken( 4 ) == 0 );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "" );
	}

	SECTION( "Whitespace#2..." )
	{
		int                    dummy;
		Cpl::Text::FString<64> token;
		Cpl::Text::FString<64> rawString = " \t ";
		Basic                  parser( rawString.getBuffer( dummy ) );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() );
	}

	SECTION( "Whitespace#3..." )
	{
		int                    dummy;
		Cpl::Text::FString<64> token;
		Cpl::Text::FString<64> rawString = " \t1 2 3    ";
		Basic                  parser( rawString.getBuffer( dummy ) );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "1" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "2" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "3" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() );


	}

	SECTION( "Delimiter set#1..." )
	{
		int                    dummy;
		Cpl::Text::FString<64> token;
		Cpl::Text::FString<64> rawString = "bob's,  (your). \n uncle";
		Basic                  parser( rawString.getBuffer( dummy ), DELIMITERS_ );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		REQUIRE( parser.numTokens() == 1 );
		REQUIRE( parser.getToken( 1 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "  (your). \n uncle" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "  " );
		REQUIRE( parser.numTokens() == 2 );
		REQUIRE( parser.getToken( 2 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.getToken( 1 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "  " );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "your). \n uncle" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "your" );
		REQUIRE( parser.numTokens() == 3 );
		REQUIRE( parser.getToken( 3 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.getToken( 1 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "  " );
		token = parser.getToken( 2 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "your" );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == ". \n uncle" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == " \n uncle" );
		REQUIRE( parser.numTokens() == 4 );
		REQUIRE( parser.getToken( 4 ) == 0 );
		token = parser.getToken( 0 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "bob's" );
		token = parser.getToken( 1 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "  " );
		token = parser.getToken( 2 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "your" );
		token = parser.getToken( 3 );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == " \n uncle" );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() );
		REQUIRE( parser.numTokens() == 4 );
		REQUIRE( parser.getToken( 4 ) == 0 );
		token = parser.remaining();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "  remaining=[%s]", token.getString() ) );
		REQUIRE( token == "" );
	}

	SECTION( "Delimiter set#2..." )
	{
		int                    dummy;
		Cpl::Text::FString<64> token;
		Cpl::Text::FString<64> rawString = DELIMITERS_ DELIMITERS_;
		Basic                  parser( rawString.getBuffer( dummy ), DELIMITERS_ );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() );
	}

	SECTION( "Delimiter set#3..." )
	{
		int                    dummy;
		Cpl::Text::FString<64> token;
		Cpl::Text::FString<64> rawString = "((1,.2), 3..(),";
		Basic                  parser( rawString.getBuffer( dummy ), DELIMITERS_ );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "1" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == "2" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() == false );
		REQUIRE( token == " 3" );

		token = parser.next();
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "token=[%s]", token.getString() ) );
		REQUIRE( token.isEmpty() );
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


















