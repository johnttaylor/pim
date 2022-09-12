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
#include "Storm/Utils/SimHouse.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"

using namespace Storm::Utils;

#define SECT_       "_0test"

#define MAX_LIMIT   (60*60*1) // 1 hour

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "SimHouse" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "Natural Cooling" )
    {
        SimHouse uut( 1.0, 80.0, 120.0, 20.0, 0.33, 0.80 );
        double idt = 0.0;
        int    i   = 0;
        while ( ( idt > 70.1 || idt < 69.9 ) && i < MAX_LIMIT )
        {
            idt = uut.tick( 70.0, 0.0, true );
            i++;
        }
        printf( "i=%d, idt=%g\n", i, idt );
        REQUIRE( ( idt <= 70.1 && idt >= 69.9 ) );
    }

    SECTION( "Natural Heating" )
    {
        SimHouse uut( 1.0, 60, 120.0, 20.0, 0.33, 0.80 );
        double idt = 0.0;
        int    i   = 0;
        while ( ( idt > 70.1 || idt < 69.9 ) && i < MAX_LIMIT )
        {
            idt = uut.tick( 70.0, 0.0, true );
            i++;
        }
        printf( "i=%d, idt=%g\n", i, idt );
        REQUIRE( ( idt <= 70.1 && idt >= 69.9 ) );
    }

    SECTION( "Active Cooling" )
    {
        SimHouse uut( 1.0, 90, 120.0, 20.0, 0.33, 0.80 );
        double idt = 0.0;
        int    i   = 0;
        while ( ( idt > 70.1 || idt < 69.9 ) && i < MAX_LIMIT )
        {
            idt = uut.tick( 90, 1.0, true );
            i++;
        }
        printf( "i=%d, idt=%g\n", i, idt );
        REQUIRE( ( idt <= 70.1 && idt >= 69.9 ) );
    }

    SECTION( "Active Heating" )
    {
        SimHouse uut( 1.0, 20, 120.0, 20.0, 0.33, 0.80 );
        double idt = 0.0;
        int    i   = 0;
        while ( ( idt > 70.1 || idt < 69.9 ) && i < MAX_LIMIT )
        {
            idt = uut.tick( 20, 1.0, false );
            i++;
        }
        printf( "i=%d, idt=%g\n", i, idt );
        REQUIRE( ( idt <= 70.1 && idt >= 69.9 ) );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

