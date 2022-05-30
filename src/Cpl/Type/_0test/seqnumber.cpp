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
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Type/SeqNumber.h"


/// 
using namespace Cpl::Type;



////////////////////////////////////////////////////////////////////////////////

#define SECT_               "_0test"



/////////////////////////////
TEST_CASE( "Sequence Number" )
{

    Cpl::System::Shutdown_TS::clearAndUseCounter();
    SeqNumber seqnum( 0 );
    SeqNumber seqnum2;

    REQUIRE( seqnum.isValid() == false );
    REQUIRE( seqnum2.isValid() == true );

    int32_t value = seqnum;
    REQUIRE( value == 0 );
    REQUIRE( seqnum.compare( 0 ) == false );            // The compare functions always return false if seqnum is zero
    REQUIRE( seqnum.compare( seqnum2 ) == false );      // The compare functions always return false if seqnum is zero
    REQUIRE( seqnum2.compare( seqnum ) == false );      // The compare functions always return false if seqnum is zero

    seqnum.increment();
    REQUIRE( seqnum.compare( 1 ) == true );
    REQUIRE( seqnum.compare( seqnum2 ) == true );

    seqnum2.set( 0x7FFFFFFF );
    REQUIRE( seqnum2.compare( 0x7FFFFFFF ) == true );
    seqnum2.increment();
    REQUIRE( seqnum2.compare( 1 ) == true );

    seqnum2.invalidate();
    REQUIRE( seqnum2.isValid() == false );
    seqnum2 = seqnum;
    REQUIRE( seqnum2.isValid() == true );

    seqnum.set( 0 );
    REQUIRE( seqnum2.isValid() == true );
    REQUIRE( seqnum2.compare( 1 ) == true );

    REQUIRE( seqnum2 == seqnum );
    seqnum2.increment();
    REQUIRE( seqnum2 != seqnum );
    REQUIRE( seqnum2 != 1 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
