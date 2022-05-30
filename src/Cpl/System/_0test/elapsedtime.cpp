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
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "elaspedtime", "[elaspedtime]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();

    unsigned long            secs;
    unsigned long            msecs;
    ElapsedTime::Precision_T precision;
    ElapsedTime::Precision_T precValue;

    // Get current time
    secs      = ElapsedTime::seconds();
    msecs     = ElapsedTime::milliseconds();
    precision = ElapsedTime::precision();

    // Sleep at least 1.5 second
    Api::sleep( 1510 );
    //unsigned long n = ElapsedTime::milliseconds();
    //printf( "now=%lu, msec=%lu, delta=%lu\n", n, msecs, n - msecs );

    // Verify delta time
    REQUIRE( ElapsedTime::expiredMilliseconds( msecs, 1500 ) == true );
    REQUIRE( ElapsedTime::expiredSeconds( secs, 1 ) == true );
    precValue.m_seconds     = 1;
    precValue.m_thousandths = 500;
    REQUIRE( ElapsedTime::expiredPrecision( precision, precValue ) == true );
    precision.m_thousandths++;
    REQUIRE( precValue != precision );
    precision.m_seconds     = 1;
    precision.m_thousandths = 500;
    REQUIRE( precValue == precision );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Post verify: sleep = 1.5") );

    // Get current time
    precision = ElapsedTime::precision();
    secs      = ElapsedTime::seconds();
    msecs     = ElapsedTime::milliseconds();

    // Sleep at least 1.0 second
    Api::sleep( 1010 );

    // Verify delta time
    REQUIRE( ElapsedTime::expiredMilliseconds( msecs, 1000 ) == true );
    REQUIRE( ElapsedTime::expiredSeconds( secs, 1 ) == true );
    precValue.m_seconds     = 1;
    precValue.m_thousandths = 0;
    REQUIRE( ElapsedTime::expiredPrecision( precision, precValue ) == true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Post verify: sleep = 1.0") );

    // Get current time
    msecs     = ElapsedTime::milliseconds();
    secs      = ElapsedTime::seconds();
    precision = ElapsedTime::precision();

    // Sleep at least 3.7 second
    Api::sleep( 3710 );

    // Verify delta time
    REQUIRE( ElapsedTime::expiredMilliseconds( msecs, 3700 ) == true );
    REQUIRE( ElapsedTime::expiredSeconds( secs, 3 ) == true );
    precValue.m_seconds     = 3;
    precValue.m_thousandths = 700;
    REQUIRE( ElapsedTime::expiredPrecision( precision, precValue ) == true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Post verify: sleep = 3.7") );

    CPL_SYSTEM_TRACE_MSG( SECT_, ("msec=%lu, secs=%lu, prec.sec=%lu, prec.msec=%u", msecs, secs, precision.m_seconds, precision.m_thousandths) );

    int i;
    for ( i=0; i < 50; i++ )
    {
        Api::sleep( 21 );
        msecs     = ElapsedTime::milliseconds();
        secs      = ElapsedTime::seconds();
        precision = ElapsedTime::precision();

        unsigned long prec_as_ms = precision.m_seconds * 1000 + precision.m_thousandths;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("msec(%lu) =? prec(%lu)", msecs, prec_as_ms) );
        REQUIRE( msecs <= prec_as_ms );
        REQUIRE( secs <= precision.m_seconds );
    }

    // Comparison operators
    ElapsedTime::Precision_T timeA = { 10, 11 };
    ElapsedTime::Precision_T timeB = { 10, 12 };
    REQUIRE( timeA < timeB );
    REQUIRE( timeA <= timeB );
    REQUIRE( timeB > timeA );
    REQUIRE( timeB >= timeA );
    REQUIRE( timeA != timeB );
    REQUIRE( (timeA == timeB) == false );

    timeA = timeB;
    REQUIRE( (timeA < timeB) == false );
    REQUIRE( timeA <= timeB );
    REQUIRE( (timeB > timeA) == false );
    REQUIRE( timeB >= timeA );
    REQUIRE( (timeA != timeB) == false );
    REQUIRE( timeA == timeB  );

    timeA = { 12, 10 };
    timeB = { 13, 10 };
    REQUIRE( timeA < timeB );
    REQUIRE( timeA <= timeB );
    REQUIRE( timeB > timeA );
    REQUIRE( timeB >= timeA );
    REQUIRE( timeA != timeB );
    REQUIRE( ( timeA == timeB ) == false );

    timeA = timeB;
    REQUIRE( ( timeA < timeB ) == false );
    REQUIRE( timeA <= timeB );
    REQUIRE( ( timeB > timeA ) == false );
    REQUIRE( timeB >= timeA );
    REQUIRE( ( timeA != timeB ) == false );
    REQUIRE( timeA == timeB );

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
