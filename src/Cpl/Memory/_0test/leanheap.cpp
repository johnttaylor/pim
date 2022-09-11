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

#include "Catch/catch.hpp"
#include "Cpl/Memory/LeanHeap.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"


#define SECT_   "_0test"

/// 
using namespace Cpl::Memory;

#define WORD_SIZE	(sizeof(size_t))

#define NUM_WORDS	100	// aka 400 bytes

static size_t memoryBlock_[NUM_WORDS];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "allocateOnly" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    LeanHeap uut( memoryBlock_, sizeof( memoryBlock_ ) );

    size_t   allocated;
    uint8_t* start = uut.getMemoryStart( allocated );
    REQUIRE( start == (uint8_t*) memoryBlock_ );
    REQUIRE( allocated == 0 );

    // Allocate...
    uint8_t* ptr = (uint8_t*)uut.allocate( 1 );
    size_t   ptrInt = (size_t)ptr;
    REQUIRE( ptr != 0 );
    REQUIRE( (ptrInt % WORD_SIZE) == 0 );
    start = uut.getMemoryStart( allocated );
    REQUIRE( start == (uint8_t*) memoryBlock_ );
    REQUIRE( allocated == 1* sizeof(size_t) );

    uint8_t* ptr2 = (uint8_t*)uut.allocate( 2 );
    REQUIRE( ptr2 != 0 );
    ptrInt = (size_t)ptr2;
    REQUIRE( (ptrInt % WORD_SIZE) == 0 );
    REQUIRE( (ptr + WORD_SIZE) == ptr2 );
    uut.getMemoryStart( allocated );
    REQUIRE( allocated == (1+1) * sizeof( size_t ) );

    // Over allocate
    ptr = (uint8_t*)uut.allocate( (NUM_WORDS * WORD_SIZE) );
    REQUIRE( ptr == 0 );
    ptr = (uint8_t*)uut.allocate( (NUM_WORDS - 1) * WORD_SIZE );
    REQUIRE( ptr == 0 );
    ptr = (uint8_t*)uut.allocate( (NUM_WORDS - 2) * WORD_SIZE );
    REQUIRE( ptr != 0 );

    // Reset heap
    ptr = (uint8_t*)uut.allocate( WORD_SIZE );
    REQUIRE( ptr == 0 );
    uut.reset();
    start = uut.getMemoryStart( allocated );
    REQUIRE( start == (uint8_t*) memoryBlock_ );
    REQUIRE( allocated == 0 );
    ptr = (uint8_t*) uut.allocate( (NUM_WORDS * WORD_SIZE) );
    REQUIRE( ptr != 0 );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}