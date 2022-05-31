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
#include "Cpl/Container/RingBufferMP.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Container;
/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////

static int memoryRBuf_[5];

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::Mp::Uint32       mp_apple_( modelDb_, "APPLE1"  );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RINGBUFFER-MP: Validate member functions", "[ringbuffer]" )
{
    RingBufferMP<int> buffer( sizeof( memoryRBuf_ ) / sizeof( memoryRBuf_[0] ), memoryRBuf_, mp_apple_ );

    Shutdown_TS::clearAndUseCounter();

    SECTION( "Operations" )
    {
        int item = 0;
        uint32_t count;

        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( mp_apple_.isNotValid() );
        item = 10;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 10 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        item = 20;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 2 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 20 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        item = 30;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 3 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 30 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        item = 40;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 40 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        item = 50;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        REQUIRE( buffer.getNumItems() == 5 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 50 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        item = 60;
        REQUIRE( buffer.add( item ) == false );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        REQUIRE( buffer.getNumItems() == 5 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 50 );


        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 20 );
        REQUIRE( *( buffer.peekTail() ) == 50 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 10 );
        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 3 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 30 );
        REQUIRE( *( buffer.peekTail() ) == 50 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 20 );
        item = 60;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 30 );
        REQUIRE( *( buffer.peekTail() ) == 60 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        item = 70;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        REQUIRE( buffer.getNumItems() == 5 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 30 );
        REQUIRE( *( buffer.peekTail() ) == 70 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );


        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 40 );
        REQUIRE( *( buffer.peekTail() ) == 70 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 30 );
        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 3 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 50 );
        REQUIRE( *( buffer.peekTail() ) == 70 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 40 );
        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 2 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 60 );
        REQUIRE( *( buffer.peekTail() ) == 70 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 50 );
        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 70 );
        REQUIRE( *( buffer.peekTail() ) == 70 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 60 );
        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        REQUIRE( item == 70 );

        REQUIRE( buffer.remove( item ) == false );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        item = 10;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 10 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
        item = 20;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 2 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 20 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );

        buffer.clearTheBuffer();
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        mp_apple_.read( count );
        REQUIRE( count == buffer.getNumItems() );
    }

    SECTION( "sequence number" )
    {
        int item = 0;
        uint32_t count;
        uint16_t seqNum1;
        uint16_t seqNum2;

        seqNum1 = mp_apple_.getSequenceNumber();
        item    = 10;
        REQUIRE( buffer.add( item ) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *( buffer.peekHead() ) == 10 );
        REQUIRE( *( buffer.peekTail() ) == 10 );
        mp_apple_.read( count, &seqNum2 );
        REQUIRE( seqNum1 + 1 == seqNum2 );
        REQUIRE( count == buffer.getNumItems() );

        REQUIRE( buffer.remove( item ) == true );
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        mp_apple_.read( count, &seqNum1 );
        REQUIRE( seqNum2 + 1 == seqNum1 );
        REQUIRE( count == buffer.getNumItems() );

        item    = 11;
        REQUIRE( buffer.add( item, seqNum1 ) == true );
        buffer.clearTheBuffer( seqNum2 );
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( seqNum1 + 1 == seqNum2 );
        mp_apple_.read( count, &seqNum1 );
        REQUIRE( seqNum1 == seqNum2 );
        REQUIRE( count == buffer.getNumItems() );
    }

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
