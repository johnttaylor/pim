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

#include "colony_config.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/File/Output.h"
#include "Cpl/Io/LineReader.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Assert.h"
#include "Cpl/Io/File/Arduino/_sdFat/Private_.h"

#define SECT_     "_0test"

#define REQUIRE   CPL_SYSTEM_ASSERT


/** Hack to get around the fact the SCM systems will convert newline
    characters in Text Files based on the target host, i.e. on Windows my
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length.
 */

#define TESTINPUT_TXT_FILE_LENGTH   101
#define TESTINPUT_TEXT_HELLO_OFFEST 0x58

void testcase_read1();
void testcase_read2();


/// 
using namespace Cpl::Io::File;

#define FILENAME    "tinput.txt"

#define FILE_CONTENTS   "line 1\n" \
                        "line 2\n" \
                        "\n" \
                        "line 4 - extra stuff, really extra stuff, okay really extra stuff\n" \
                        "line 5\n" \
                        "AHello World."

////////////////////////////////////////////////////////////////////////////////
void testcase_read1()
{
    // Clean-up from previous tests
    g_arduino_sdfat_fs.chdir( true );
    g_arduino_sdfat_fs.vwd()->rmRfStar();

    // Create input file
    Output fdout( FILENAME, true, true);
    REQUIRE( fdout.isOpened() );
    fdout.write( FILE_CONTENTS );
    fdout.close();

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Reading Lines...") );
    Input fd( FILENAME );
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
    unsigned long len;
    REQUIRE( fd.length( len ) == true );
    REQUIRE( len == TESTINPUT_TXT_FILE_LENGTH );
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
    REQUIRE( fd.isOpened() == false );
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( fd.read( line ) == false );

    //     
    Input fd2( FILENAME );
    REQUIRE( fd2.isOpened() );
    char dummyChar = 29;
    REQUIRE( fd2.setAbsolutePos( TESTINPUT_TEXT_HELLO_OFFEST ) );
    REQUIRE( fd2.length( len ) == true );
    REQUIRE( len == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( fd2.read( dummyChar ) == true );
    REQUIRE( dummyChar == 'A' );

    Cpl::Text::FString<10> buffer( "bob" );
    REQUIRE( fd2.read( buffer ) == true );
    REQUIRE( buffer == "Hello Worl" );

    char myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int  bytesRead    = 1;
    REQUIRE( fd2.available() == true );
    REQUIRE( fd2.read( myBuffer, sizeof( myBuffer ), bytesRead ) == true );
    REQUIRE( fd2.isEof() == false );
    REQUIRE( bytesRead == 2 );
    REQUIRE( myBuffer[0] == 'd' );
    REQUIRE( myBuffer[1] == '.' );
    REQUIRE( fd2.read( myBuffer, sizeof( myBuffer ), bytesRead ) == false );
    REQUIRE( fd2.isEof() == true );
    REQUIRE( bytesRead == 0 );

    fd2.close();
    REQUIRE( fd2.isOpened() == false );
    REQUIRE( fd2.read( dummyChar ) == false );
}

void testcase_read2()
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Input fd( FILENAME );
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
}
