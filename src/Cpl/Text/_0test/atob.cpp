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
#include "Cpl/Text/atob.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <stdint.h>
#include "colony_map.h"


/// 
using namespace Cpl::Text;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "atob", "[atob]" )
{

    Shutdown_TS::clearAndUseCounter();

    SECTION( "conversions...." )
    {
        const char* strPtr = "12.3";
        const char* endPtr = 0;
        int value;

        bool result = a2i( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2i( value, " 1234 " );
        REQUIRE( value == 123 );	// Conversion failed -->value should not be changed
        REQUIRE( result == false );
        result = a2i( value, " -1234(" );
        REQUIRE( value == 123 );	// Conversion failed -->value should not be changed
        REQUIRE( result == false );
        result = a2i( value, "12.3", 10, ":,." );
        REQUIRE( value == 12 );
        REQUIRE( result == true );
        result = a2i( value, strPtr, 10, ".,", &endPtr );
        REQUIRE( value == 12 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 2 );
        result = a2i( value, "1.23" );
        REQUIRE( value == 12 );	// Conversion failed -->value should not be changed
        REQUIRE( result == false );

        result = a2i( value, "0xF" );
        REQUIRE( value == 12 );	// Conversion failed -->value should not be changed
        REQUIRE( result == false );
        result = a2i( value, "0xF", 0 );
        REQUIRE( value == 0xF );
        REQUIRE( result == true );

        result = a2i( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "unsigned..." )
    {
        const char* strPtr = "-123";
        const char* endPtr = 0;
        unsigned value;

        bool result = a2ui( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2ui( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == ( unsigned) -123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 4 );
        result = a2ui( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "long..." )
    {
        const char* strPtr = "123";
        const char* endPtr = 0;
        long value;

        bool result = a2l( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2l( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 3 );
        result = a2l( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "unsigned long..." )
    {
        const char* strPtr = "-123";
        const char* endPtr = 0;
        unsigned long value;

        bool result = a2ul( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2ul( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == ( unsigned long) -123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 4 );
        result = a2ul( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "long long..." )
    {
        const char* strPtr = "123";
        const char* endPtr = 0;
        long long value;

        bool result = a2ll( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = a2ll( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 3 );
        result = a2ll( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "unsigned long long..." )
    {
        const char* strPtr = "-123";
        const char* endPtr = 0;
        unsigned long long value;

        bool result = a2ull( value, "123" );
        REQUIRE( value == 123u );
        REQUIRE( result == true );
        result = a2ull( value, strPtr, 10, 0, &endPtr );
        REQUIRE( value == ( unsigned long long ) - 123 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 4 );
        result = a2ull( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "double..." )
    {
        const char* strPtr = "-123.32";
        const char* endPtr = 0;
        double value;

        bool result = a2d( value, "123.32" );
        REQUIRE( value == 123.32 );
        REQUIRE( result == true );
        result = a2d( value, strPtr, 0, &endPtr );
        REQUIRE( value == -123.32 );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 7 );
        result = a2d( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "bool" )
    {
        const char* strPtr = "false";
        const char* endPtr = 0;
        bool value;

        bool result = a2b( value, "T" );
        REQUIRE( value == true );
        REQUIRE( result == true );
        result = a2b( value, strPtr, "true", "false", &endPtr );
        REQUIRE( value == false );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 5 );
        result = a2b( value, 0 );
        REQUIRE( result == false );
    }

    SECTION( "hextobuffer" )
    {
        const char* text = "AA0055BBFC";
        uint8_t buffer[5];

        long result = asciiHexToBuffer( buffer, text, sizeof( buffer ) );
        REQUIRE( result == 5 );
        REQUIRE( buffer[0] == 0xAA );
        REQUIRE( buffer[1] == 0x00 );
        REQUIRE( buffer[2] == 0x55 );
        REQUIRE( buffer[3] == 0xBB );
        REQUIRE( buffer[4] == 0xFC );

        result = asciiHexToBuffer( buffer, 0, sizeof( buffer ) );
        REQUIRE( result == -1 );
        result = asciiHexToBuffer( 0, text, sizeof( buffer ) );
        REQUIRE( result == -1 );
        result = asciiHexToBuffer( buffer, text, 0 );
        REQUIRE( result == -1 );

        result = asciiHexToBuffer( buffer, "AA3", sizeof( buffer ) );
        REQUIRE( result == -1 );

        result = asciiHexToBuffer( buffer, text, 3 );
        REQUIRE( result == -1 );
    }

    SECTION( "parse Precision time" )
    {
        const char* text = "12:13:14.5";
        Cpl::System::ElapsedTime::Precision_T time;
        bool result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time.m_thousandths == 500 );
        REQUIRE( time.m_seconds == 14 + ( 13 * 60 ) + ( 12 * 60 * 60 ) );

        text   = "02 12:13:14.05";
        result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time.m_thousandths == 50 );
        REQUIRE( time.m_seconds == 14 + ( 13 * 60 ) + ( 12 * 60 * 60 ) + ( 2 * 60 * 60 * 24 ) );

        text   = "2 12:3:14.005";
        result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time.m_thousandths == 5 );
        REQUIRE( time.m_seconds == 14 + ( 3 * 60 ) + ( 12 * 60 * 60 ) + ( 2 * 60 * 60 * 24 ) );

        text   = "02 12:13:14";
        result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == true );

        text   = "12:13:14";
        result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == true );

        text   = "02-12:13:14";
        result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == false );
        
        text   = "02-12:13:14.1234";
        result = parsePrecisionTimeStamp( text, time );
        REQUIRE( result == false );

    }

    SECTION( "bintobuffer" )
    {
        const char* text = "00101100101001011111000100011111"; // 0x2CA5F11F
        uint8_t buffer[4];

        long result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == 4*8 );
        REQUIRE( buffer[0] == 0x1F );
        REQUIRE( buffer[1] == 0xF1 );
        REQUIRE( buffer[2] == 0xA5 );
        REQUIRE( buffer[3] == 0x2C );
        
        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ) );
        REQUIRE( result == 4*8 );
        REQUIRE( buffer[0] == 0x2C );
        REQUIRE( buffer[1] == 0xA5 );
        REQUIRE( buffer[2] == 0xF1 );
        REQUIRE( buffer[3] == 0x1F );

        result = asciiBinaryToBuffer( buffer, 0, sizeof( buffer ) );
        REQUIRE( result == -1 );
        result = asciiBinaryToBuffer( 0, text, sizeof( buffer ) );
        REQUIRE( result == -1 );
        result = asciiBinaryToBuffer( buffer, text, 0 );
        REQUIRE( result == -1 );

        result = asciiBinaryToBuffer( buffer, text, 1 );
        REQUIRE( result == 8 );
        REQUIRE( buffer[0] == 0x2C );

        uint8_t buffer2[5];
        result = asciiBinaryToBuffer( buffer2, text, sizeof( buffer2 ), true );
        REQUIRE( result == 4*8 );
        REQUIRE( buffer2[0] == 0x1F );
        REQUIRE( buffer2[1] == 0xF1 );
        REQUIRE( buffer2[2] == 0xA5 );
        REQUIRE( buffer2[3] == 0x2C );

        result = asciiBinaryToBuffer( buffer2, text, sizeof( buffer2 ) );
        REQUIRE( result == 4*8 );
        REQUIRE( buffer2[0] == 0x2C );
        REQUIRE( buffer2[1] == 0xA5 );
        REQUIRE( buffer2[2] == 0xF1 );
        REQUIRE( buffer2[3] == 0x1F );

        text = "10101"; // 0xA8
        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ) );
        REQUIRE( result == 5 );
        REQUIRE( buffer[0] == 0xA8 );

        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == 5 );
        REQUIRE( buffer[0] == 0xA8 );

        text = "100 1"; // error
        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ) );
        REQUIRE( result == -1 );

        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == -1 );

        text = " 1001"; // error
        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ) );
        REQUIRE( result == -1 );

        result = asciiBinaryToBuffer( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == -1 );
    }

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
