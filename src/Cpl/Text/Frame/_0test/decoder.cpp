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
#include "Cpl/Text/Frame/AsciiDecoder.h"
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
TEST_CASE( "decode", "[decode]" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	Cpl::Io::File::Input  infd( "testinput.txt" );
	AsciiDecoder<7>       decoder( SOF_, EOF_, ESC_, true, &infd );
	size_t                fsize;

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "hello world" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "good frame" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "just kidding" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".sof" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".more sof" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ";eof" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".~;sef" );

	infd.close();
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


















