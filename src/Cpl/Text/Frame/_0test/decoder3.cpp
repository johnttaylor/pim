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
#include "Cpl/Text/Frame/StringDecoder.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text::Frame;


#define SECT_   "_0test"

////////////////////////////////////////////////////////////////////////////////


#ifndef SOF_
#define SOF_     '.'
#endif
#ifndef EOF_
#define EOF_     ';'
#endif
#ifndef ESC_
#define ESC_     '~'
#endif

#ifndef BUFSIZE_
#define BUFSIZE_     16
#endif

static Cpl::Text::FString<BUFSIZE_ + 1> instring;
static char                           buffer_[BUFSIZE_];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "stringdecoder", "[stringdecoder]" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	StringDecoder   decoder( SOF_, EOF_, ESC_ );
	const char*     endPtr;
	size_t          fsize;

	// test null pointer error
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) == false );

	decoder.setInput( ".hello world;" );
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "hello world" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == '\0' );

	// Test scan of exhausted string
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) == false );

	decoder.setInput( ".start" );
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) == false );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "start" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == '\0' );

	decoder.setInput( ".this frame exceeds client's buffer;.just kidding;" );
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "just kidding" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == '\0' );

	decoder.setInput( "..sof; .~.more sof;." );
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".sof" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == ' ' );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".more sof" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == '.' );

	decoder.setInput( "not-rejected;" );
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "not-rejected" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == '\0' );

	decoder.setInput( ".~;eof;x.~.~~~;sef;y" );
	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ";eof" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == 'x' );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".~;sef" );
	endPtr = decoder.getRemainder();
	REQUIRE( *endPtr == 'y' );

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );
}


















