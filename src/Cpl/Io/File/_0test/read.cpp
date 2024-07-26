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
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/LineReader.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"


#define SECT_     "_0test"


/** Hack to get around the fact the SCM systems will convert newline
    characters in Text Files based on the target host, i.e. on Windows my
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length.
 */
#define WIN32_TESTINPUT_TXT_FILE_LENGTH   106
#define WIN32_TESTINPUT_TEXT_HELLO_OFFEST 0x5D
#define POSIX_TESTINPUT_TXT_FILE_LENGTH   101
#define POSIX_TESTINPUT_TEXT_HELLO_OFFEST 0x58


 /// 
using namespace Cpl::Io::File;



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "read", "[read]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Reading Lines...") );
    Input fd( "testinput.txt" );
    Cpl::Io::LineReader reader( fd );
    REQUIRE( fd.isOpened() );

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

    unsigned long len;
    REQUIRE( fd.length( len ) == true );
    REQUIRE( (len == WIN32_TESTINPUT_TXT_FILE_LENGTH || len == POSIX_TESTINPUT_TXT_FILE_LENGTH) );

    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    REQUIRE( (line == "line 4" || line == ""));

    REQUIRE( reader.readln( line ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("line=[%s]", line.getString()) );
    REQUIRE( (line == "line 5" || line == "") );

    reader.close();
    REQUIRE( fd.isOpened() == false );
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( fd.read( line ) == false );

    //     
    Input fd2( "testinput.txt" );
    REQUIRE( fd2.isOpened() );
    char dummyChar = 29;
    REQUIRE( (fd2.setAbsolutePos( WIN32_TESTINPUT_TEXT_HELLO_OFFEST ) || fd2.setAbsolutePos( POSIX_TESTINPUT_TEXT_HELLO_OFFEST ) ));
    REQUIRE( fd2.length( len ) == true );
    REQUIRE( (len == WIN32_TESTINPUT_TXT_FILE_LENGTH ||  len == POSIX_TESTINPUT_TXT_FILE_LENGTH) );
    REQUIRE( fd2.read( dummyChar ) == true );
    REQUIRE( (dummyChar == 'A' || dummyChar == 'o') );

    Cpl::Text::FString<10> buffer( "bob" );
    REQUIRE( fd2.read( buffer ) == true );
    printf( "buff=%s", buffer.getString() );
    REQUIRE( (buffer == "Hello Worl" || buffer == " World.") );

    fd2.close();
    REQUIRE( fd2.isOpened() == false );
    REQUIRE( fd2.read( dummyChar ) == false );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "read/close", "[read/close]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Input fd( "testinput.txt" );
    REQUIRE( fd.isOpened() );
    fd.close();
    char dummyChar = 29;
    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    REQUIRE( fd.available() == false );

    REQUIRE( fd.isEof() == true );
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == false );
    REQUIRE( fd.setAbsolutePos( 1 ) == false );
    REQUIRE( fd.setRelativePos( 1 ) == false );
    REQUIRE( fd.setToEof() == false );
    unsigned long len = 22;
    REQUIRE( fd.length( len ) == false );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
