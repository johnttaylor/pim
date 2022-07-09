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
#include "Cpl/Text/Frame/StringEncoder.h"
#include "Cpl/Text/Frame/AsciiDecoder.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/File/Output.h"
#include "Cpl/Io/LineReader.h"
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

#ifndef OUTSIZE_
#define OUTSIZE_     128
#endif

#define OUTFILE_NAME_   "output2.txt"

static Cpl::Text::FString<BUFSIZE_ + 1> instring;
static char                           buffer_[BUFSIZE_];
static Cpl::Text::FString<OUTSIZE_ + 1> outputFrame_;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "encode2", "[encode2]" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	Cpl::Io::File::Output outfd( OUTFILE_NAME_ );

	{
		StringEncoder encoder( outputFrame_, SOF_, EOF_, ESC_ );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.output( "hello world" ) );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.output( ".~;" ) );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.output( "A~~;;.Z" ) );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );
	}


	{
		StringEncoder encoder( outputFrame_, SOF_, EOF_, ESC_, false );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.output( "hello world" ) );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.output( ".~;" ) );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );

		REQUIRE( encoder.startFrame() );
		REQUIRE( encoder.output( "A~~;;.Z" ) );
		REQUIRE( encoder.endFrame() );
		REQUIRE( outfd.write( outputFrame_ ) );
	}

	outfd.close();
	Cpl::Io::File::Input    infd( OUTFILE_NAME_ );
	Cpl::Io::LineReader     reader( infd );
	Cpl::Text::FString<256> instring;

	REQUIRE( reader.readln( instring ) );
	REQUIRE( instring == ".hello world;" );

	REQUIRE( reader.readln( instring ) );
	REQUIRE( instring == "..~~~;;" );

	REQUIRE( reader.readln( instring ) );
	REQUIRE( instring == ".;" );

	REQUIRE( reader.readln( instring ) );
	REQUIRE( instring == ".A~~~~~;~;.Z;" );

	REQUIRE( reader.readln( instring ) == false );
	REQUIRE( instring == ".hello world;..~~~;;.;.A~~~~~;~;.Z;" );
	infd.close();

	{
		Cpl::Io::File::Input  infd( OUTFILE_NAME_ );
		AsciiDecoder<128>     decoder( SOF_, EOF_, ESC_, true, &infd );
		size_t                fsize;

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == "hello world" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == ".~;" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == "" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == "A~~;;.Z" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == "hello world" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == ".~;" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == "" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) );
		instring.copyIn( buffer_, fsize );
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Frame=[%s]", instring.getString() ) );
		REQUIRE( instring == "A~~;;.Z" );

		REQUIRE( decoder.scan( sizeof( buffer_ ), buffer_, fsize ) == false );
		REQUIRE( fsize == 0u );
		infd.close();
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


















