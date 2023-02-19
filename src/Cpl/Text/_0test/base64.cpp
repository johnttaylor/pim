/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*
* ------------------------------------------------------------------------------
* This code is an adaptation of a module written by:
*
*   Copyright (c) 2013 Adam Rudd.
*   https://github.com/adamvr
*
* The original code is MIT Licensed:
*
* Copyright (C) 2013 Adam Rudd
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
* of the Software, and to permit persons to whom the Software is furnished to do
* so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* ------------------------------------------------------------------------------
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/Text/base64.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define SECT_   "_0test"

/// 
using namespace Cpl::Text;
using namespace Cpl::System;


#define GOLDEN_IN           "Hello world"
#define EXPECTED_OUT        "SGVsbG8gd29ybGQ="  

#define GOLDEN_OUT          "Zm9vYmFy"
#define EXPECTED_IN         "foobar"

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "base64" )
{

    Shutdown_TS::clearAndUseCounter();


    SECTION( "golden" )
    {
        size_t  payloadSize = strlen(GOLDEN_IN);
        char    base64[32];
        int     outLen = base64Encode( base64, sizeof(base64), GOLDEN_IN, payloadSize );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "encoded=[%s]", base64 ) );
        REQUIRE( strcmp( base64, EXPECTED_OUT ) == 0 );
        REQUIRE( outLen == strlen(EXPECTED_OUT) );

        char     binary[32];
        outLen = base64Decode( binary, sizeof( binary ), GOLDEN_OUT, strlen(GOLDEN_OUT) );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "decoded=[%.*s]", outLen, binary ) );
        REQUIRE( strncmp( binary, EXPECTED_IN, outLen ) == 0 );
        REQUIRE( outLen == strlen(EXPECTED_IN) );
    }

    SECTION( "Encode: Error cases" )
    {
        char inbuf[32];
        char outbuf[32];
        int  outLen = base64Encode( nullptr, 1, inbuf, sizeof( inbuf ) );
        REQUIRE( outLen < 0 );
        outLen = base64Encode( outbuf, sizeof(outbuf), nullptr, sizeof( inbuf ) );
        REQUIRE( outLen < 0 );
        outLen = base64Encode( outbuf, sizeof(outbuf), inbuf, 0 );
        REQUIRE( outLen < 0 );
        outLen = base64Encode( outbuf, 1, inbuf, sizeof( inbuf ) );
        REQUIRE( outLen < 0 );
    }

    SECTION( "Decode: Error cases" )
    {
        char inbuf[32];
        char outbuf[32];
        int  outLen = base64Decode( nullptr, 1, inbuf, sizeof( inbuf ) );
        REQUIRE( outLen < 0 );
        outLen = base64Decode( outbuf, sizeof(outbuf), nullptr, sizeof( inbuf ) );
        REQUIRE( outLen < 0 );
        outLen = base64Decode( outbuf, sizeof(outbuf), inbuf, 0 );
        REQUIRE( outLen < 0 );
        outLen = base64Decode( outbuf, 1, inbuf, sizeof( inbuf ) );
        REQUIRE( outLen < 0 );
    }

    SECTION( "Decode: illegal characters" )
    {
        char const base64[] = { '0', -1, '1', '2',  '3', 0 };
        char outbuf[32];
        int outLen = base64Decode( outbuf, sizeof(outbuf), base64, sizeof( base64 ) - 1 );
        REQUIRE( outLen < 0 );
    }
}
