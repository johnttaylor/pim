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
#include "Cpl/Io/File/Output.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/File/InputOutput.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"
#include <time.h>
#include "Cpl/System/Assert.h"
#include "Cpl/Io/File/Arduino/_sdFat/Private_.h"

#define SECT_     "_0test"

#define REQUIRE   CPL_SYSTEM_ASSERT


#define FILE_NAME       "output1.txt"
#define OUTPUT_TXT      "Hello world!"
#define OUTPUT_TXT2     "gd-by"
#define OUTPUT_TXT3     "gd-by world!"
#define OUTPUT_TXT4     " world!"

void testcase_objectapi();
void testcase_objectapi2();
void testcase_objectapi3();

static void testcase_objectapi_absolute();
static void testcase_objectapi_relative();
static void testcase_objectapi2_absolute();
static void testcase_objectapi2_relative();
static void testcase_objectapi3_absolute();
static void testcase_objectapi3_relative();


/// 
using namespace Cpl::Io::File;

void testcase_objectapi()
{
    testcase_objectapi_absolute();
    testcase_objectapi_relative();
}

static void testcase_objectapi_absolute()
{
    // Clean-up from previous tests
    g_arduino_sdfat_fs.chdir( true );
    g_arduino_sdfat_fs.vwd()->rmRfStar();

    // SETUP
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::Text::FString<20>  buffer( "bob" );
    Output fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    // SECTION
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == 0 );
    REQUIRE( fd.write( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );

    fd.setAbsolutePos( 0 );
    REQUIRE( fd.write( OUTPUT_TXT2 ) );
    fd.flush();
    unsigned long len;
    REQUIRE( fd.length( len ) == true );
    REQUIRE( len == strlen( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT2 ) );
    REQUIRE( fd.setToEof() == true );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );
    fd.close();

    Input infd( FILE_NAME );
    REQUIRE( infd.isOpened() );
    REQUIRE( infd.length( len ) == true );
    REQUIRE( len == strlen( OUTPUT_TXT ) );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( infd.isEof() == false );
    REQUIRE( buffer == OUTPUT_TXT3 );
    REQUIRE( infd.read( buffer ) == false );
    REQUIRE( infd.isEof() == true );
    infd.close();


    // CLEAN-UP
    fd.close();
}

static void testcase_objectapi_relative()
{
    // SETUP
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::Text::FString<20>  buffer( "bob" );
    Output fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    // SECTION
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == 0 );
    REQUIRE( fd.write( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );

    long newpos = strlen( OUTPUT_TXT );
    fd.setRelativePos( -newpos );
    REQUIRE( fd.write( OUTPUT_TXT2 ) );
    fd.flush();
    fd.close();

    Input infd( FILE_NAME );
    REQUIRE( infd.isOpened() );
    unsigned long len;
    REQUIRE( infd.length( len ) == true );
    REQUIRE( len == strlen( OUTPUT_TXT ) );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( infd.isEof() == false );
    REQUIRE( buffer == OUTPUT_TXT3 );
    REQUIRE( infd.read( buffer ) == false );
    REQUIRE( infd.isEof() == true );
    infd.close();


    // CLEAN-UP
    fd.close();
}

void testcase_objectapi2()
{
    testcase_objectapi2_absolute();
    testcase_objectapi2_relative();
}

void testcase_objectapi2_absolute()
{
    // SETUP
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::Text::FString<20>  buffer( "bob" );
    Output fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    // SECTION
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == 0 );
    REQUIRE( fd.write( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );

    fd.setAbsolutePos( 0 );
    REQUIRE( fd.write( OUTPUT_TXT2 ) );
    fd.flush();
    unsigned long len;
    REQUIRE( fd.length( len ) == true );
    REQUIRE( len == strlen( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT2 ) );
    REQUIRE( fd.setToEof() == true );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );
    fd.close();

    Input infd( FILE_NAME );
    REQUIRE( infd.isOpened() );
    REQUIRE( infd.length( len ) == true );
    REQUIRE( len == strlen( OUTPUT_TXT ) );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( infd.isEof() == false );
    REQUIRE( buffer == OUTPUT_TXT3 );
    REQUIRE( infd.read( buffer ) == false );
    REQUIRE( infd.isEof() == true );
    infd.close();

    // CLEAN-UP
    fd.close();
}

void testcase_objectapi2_relative()
{
    // SETUP
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::Text::FString<20>  buffer( "bob" );
    Output fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    // SECTION
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == 0 );
    REQUIRE( fd.write( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );

    long newpos = strlen( OUTPUT_TXT );
    fd.setRelativePos( -newpos );
    REQUIRE( fd.write( OUTPUT_TXT2 ) );
    fd.flush();
    fd.close();

    Input infd( FILE_NAME );
    REQUIRE( infd.isOpened() );
    unsigned long len;
    REQUIRE( infd.length( len ) == true );
    REQUIRE( len == strlen( OUTPUT_TXT ) );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( infd.isEof() == false );
    REQUIRE( buffer == OUTPUT_TXT3 );
    REQUIRE( infd.read( buffer ) == false );
    REQUIRE( infd.isEof() == true );
    infd.close();

    // CLEAN-UP
    fd.close();
}

void testcase_objectapi3()
{
    testcase_objectapi3_absolute();
    testcase_objectapi3_relative();
}

void testcase_objectapi3_absolute()
{
    // SETUP
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::Text::FString<20>  buffer( "bob" );
    Output fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    // SECTION
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == 0 );
    REQUIRE( fd.write( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );
    fd.close();

    Input infd( FILE_NAME );
    REQUIRE( infd.setAbsolutePos( strlen( OUTPUT_TXT2 ) ) == true );
    unsigned long curpos;
    REQUIRE( infd.currentPos( curpos ) == true );
    REQUIRE( curpos == strlen( OUTPUT_TXT2 ) );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( infd.isEof() == false );
    REQUIRE( buffer == OUTPUT_TXT4 );
    REQUIRE( infd.read( buffer ) == false );
    REQUIRE( infd.isEof() == true );
    infd.close();

    // CLEAN-UP
    fd.close();
}

void testcase_objectapi3_relative()
{
    // SETUP
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::Text::FString<20>  buffer( "bob" );
    Output fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    // SECTION
    unsigned long pos;
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == 0 );
    REQUIRE( fd.write( OUTPUT_TXT ) );
    REQUIRE( fd.currentPos( pos ) == true );
    REQUIRE( pos == strlen( OUTPUT_TXT ) );
    fd.close();

    Input infd( FILE_NAME );
    REQUIRE( infd.setToEof() == true );
    REQUIRE( infd.setRelativePos( (long)( strlen( OUTPUT_TXT2 ) ) - (long)( strlen( OUTPUT_TXT ) ) ) == true );
    unsigned long curpos;
    REQUIRE( infd.currentPos( curpos ) == true );
    REQUIRE( curpos == strlen( OUTPUT_TXT2 ) );

    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( infd.isEof() == false );
    REQUIRE( buffer == OUTPUT_TXT4 );
    REQUIRE( infd.read( buffer ) == false );
    REQUIRE( infd.isEof() == true );
    infd.close();

    // CLEAN-UP
    fd.close();
}

