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
#include "Cpl/Text/Frame/LineDecoder.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Text::Frame;

#define SECT_   "_0test"

////////////////////////////////////////////////////////////////////////////////


#ifndef BUFSIZE_
#define BUFSIZE_     16
#endif

static Cpl::Text::FString<BUFSIZE_ + 1> instring;
static char                           buffer_[BUFSIZE_];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "linedecoder", "[linedecoder]" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	Cpl::Io::File::Input  infd( "testinput2.txt" );
	LineDecoder<7>        decoder( &infd );
	size_t                fsize;

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "hello world" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "  start" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == "not really end" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == ".good frame;" );

	REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
	instring.copyIn( buffer_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
	REQUIRE( instring == " just kidding  " );

	infd.close();
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


















