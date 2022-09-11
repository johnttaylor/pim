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

#include "colony_config.h"
#include "Catch/catch.hpp"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/Io/LineWriter.h"
#include "Cpl/Io/LineReader.h"
#include "Cpl/Io/TeeOutput.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"


#define SECT_     "_0test"

/// 
using namespace Cpl::Io::Stdio;



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "basic", "[basic]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Reading Lines...") );
    StdIn in2fd;
    Cpl::Io::LineReader reader( in2fd );
    Cpl::Text::FString<6> line;
    REQUIRE( reader.available() == true );
    reader.readln( line );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    REQUIRE( line == "line 1" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    REQUIRE( line == "line 2" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    line.removeTrailingSpaces();
    REQUIRE( line.isEmpty() );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    REQUIRE( line == "line 4" );
    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    REQUIRE( line == "line 5" );
    reader.close();
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( in2fd.read( line ) == false );

    //     
    StdIn infd;
    char dummyChar = 29;

    REQUIRE( infd.isOpened() == true );

    REQUIRE( infd.read( dummyChar ) == true );
    REQUIRE( dummyChar == 'A' );

    Cpl::Text::FString<10> buffer( "bob" );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( buffer == "Hello Worl" );

    char myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int  bytesRead    = 1;
    REQUIRE( infd.available() == true );
    REQUIRE( infd.read( myBuffer, 0, bytesRead ) == true );
    REQUIRE( bytesRead == 0 );
    infd.read( myBuffer, sizeof( myBuffer ), bytesRead );
    REQUIRE( bytesRead == 2 );
    REQUIRE( myBuffer[0] == 'd' );
    REQUIRE( myBuffer[1] == '.' );
    bool result = infd.read( myBuffer, sizeof( myBuffer ), bytesRead );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "result=%d, bytesRead=%d, myBuffer=[%.*s]", result, bytesRead, bytesRead, myBuffer ) );
	REQUIRE( result == false );

#ifndef SKIP_AVAILABLE_TEST // Work-around for the Stdio::Ansi implementation not implementing the available() method
    REQUIRE( infd.available() == false );
#endif

    infd.close();
    REQUIRE( infd.isOpened() == false );
    REQUIRE( infd.read( dummyChar ) == false );


    //
    StdOut outfd;
    int    bytesWritten;
    REQUIRE( outfd.isOpened() == true );
    REQUIRE( outfd.write( myBuffer, 0, bytesWritten ) == true );
    REQUIRE( bytesWritten == 0 );
    REQUIRE( outfd.write( 'a' ) );
    REQUIRE( outfd.write( "bob's your uncle" ) );
    REQUIRE( outfd.write( buffer ) );
    REQUIRE( outfd.write( buffer, "Hello %s", "World" ) );
    REQUIRE( buffer == "Hello Worl" );
    REQUIRE( outfd.write( myBuffer, sizeof( myBuffer ) ) );
    REQUIRE( outfd.write( myBuffer, sizeof( myBuffer ), bytesWritten ) );
    REQUIRE( (size_t) bytesWritten == sizeof( myBuffer ) );

    outfd.flush();
    outfd.close();
    REQUIRE( outfd.isOpened() == false );
    REQUIRE( outfd.write( 'a' ) == false );

    //
    StdOut out2fd;
    REQUIRE( out2fd.isOpened() == true );
    REQUIRE( out2fd.write( myBuffer, 0, bytesWritten ) == true );
    REQUIRE( bytesWritten == 0 );
    Cpl::Io::LineWriter writer( out2fd );
    REQUIRE( writer.println() );
    REQUIRE( writer.println( "Hello World" ) );
    REQUIRE( writer.print( "Hello" ) );
    REQUIRE( writer.print( "World" ) );
    REQUIRE( writer.print( " again!" ) );
    REQUIRE( writer.println() );
    writer.flush();
    writer.close();
    REQUIRE( out2fd.isOpened() == false );
    REQUIRE( writer.println() == false );
    REQUIRE( out2fd.write( 'a' ) == false );

    //
    StdOut out3fd;
    StdOut out4fd;
    StdOut out5fd;
    Cpl::Io::TeeOutput  outputs( out3fd );

    REQUIRE( outputs.write( "[Hello World (fd3)]\n" ) );
    outputs.add( out4fd );
    REQUIRE( outputs.write( "[Hello World - Again (fd3,fd4)]\n" ) );
    outputs.add( out5fd );
    REQUIRE( outputs.write( "[Hello World - Again - Again! (fd3,fd4,fd5)]\n" ) );
    REQUIRE( outputs.remove( out5fd ) );
    REQUIRE( outputs.write( "[Hello World - Again^2 (fd3,fd4)]\n" ) );
    outputs.close();
    REQUIRE( outputs.write( "[All Should fail]\n" ) == false );
    REQUIRE( outputs.remove( out3fd ) );
    REQUIRE( outputs.write( "[Hello World - Again]\n" ) == false );
    REQUIRE( outputs.remove( out4fd ) );
    REQUIRE( outputs.write( "[Hello World]\n" ) == false );
    REQUIRE( outputs.remove( out5fd ) == false );
    out5fd.close();
    REQUIRE( out5fd.write( "should fail because closed\n" ) == false );
    REQUIRE( out4fd.write( "should fail because closed\n" ) == false );
    REQUIRE( out3fd.write( "should fail because closed\n" ) == false );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "close", "[close]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    StdIn fd;
    char dummyChar = 29;

    fd.close();
    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    StdOut fd2;
    fd2.close();
    REQUIRE( fd2.write( 'a' ) == false );

    StdOut fd3;
    fd3.close();
    REQUIRE( fd3.write( 'a' ) == false );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "open/activate", "[open/activate]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    StdIn fd;
    Cpl::Io::Descriptor rawFd = { 0, };
    fd.activate( (int) 1 );
    fd.activate( (void*) 1 );
    fd.activate( rawFd );
    fd.close();
    fd.activate( rawFd );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 3u );

    fd.close();
    fd.activate( (int) 1 );
    fd.activate( (void*) 1 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

    StdOut fd2;
    fd2.activate( (int) 1 );
    fd2.activate( (void*) 1 );
    fd2.activate( rawFd );
    fd2.close();
    fd2.activate( rawFd );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 3u );

    fd2.close();
    fd2.activate( (int) 1 );
    fd2.activate( (void*) 1 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );
}
