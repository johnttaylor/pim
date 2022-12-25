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

#ifndef BIGGER_BUFSIZE_
#define BIGGER_BUFSIZE_     128
#endif

static Cpl::Text::FString<BUFSIZE_ + 1> instring;
static char                             buffer_[BUFSIZE_];

static Cpl::Text::FString<BIGGER_BUFSIZE_ + 1> instring2;
static char									   buffer2_[BIGGER_BUFSIZE_];

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

	REQUIRE( decoder.scan( sizeof( buffer2_ ), buffer2_, fsize ) );
	instring2.copyIn( buffer2_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ("Frame=[%s]", instring2.getString()) );
	REQUIRE( instring2 == "a real frame followed by oob" );

	int oobBytes;
	instring2.clear();
	int expectedCount = 44;
	bool result = true;
	while ( result && expectedCount )
	{
		result = decoder.oobRead( buffer2_, expectedCount, oobBytes );
		if ( result )
		{
			instring2.appendTo( buffer2_, oobBytes );
			expectedCount -= oobBytes;
		}
	}
	CPL_SYSTEM_TRACE_MSG( SECT_, ("oob=[%s]", instring2.getString()) );
	REQUIRE( instring2 == "#my oob data here and more total of 44 bytes" );

	REQUIRE( decoder.scan( sizeof( buffer2_ ), buffer2_, fsize ) );
	instring2.copyIn( buffer2_, fsize );
	CPL_SYSTEM_TRACE_MSG( SECT_, ("Frame=[%s]", instring2.getString()) );
	REQUIRE( instring2 == "next frame" );

	infd.close();
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


















