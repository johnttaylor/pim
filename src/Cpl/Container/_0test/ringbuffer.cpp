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
#include "Cpl/Container/RingBuffer.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Container;
/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////

static int memoryRBuf_[5];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RINGBUFFER: Validate member functions", "[ringbuffer]" )
    {
    RingBuffer<int> buffer( sizeof(memoryRBuf_)/sizeof(memoryRBuf_[0]), memoryRBuf_ );

    Shutdown_TS::clearAndUseCounter();


    SECTION( "Operations" )
        {
        int item = 0;
        
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        item = 10;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 10 );
        item = 20;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 2 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 20 );
        item = 30;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 3 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 30 );
        item = 40;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 40 );
        item = 50;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        REQUIRE( buffer.getNumItems() == 5 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 50 );
        item = 60;
        REQUIRE( buffer.add(item) == false );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        REQUIRE( buffer.getNumItems() == 5 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 50 );


        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 20 );
        REQUIRE( *(buffer.peekTail()) == 50 );
        REQUIRE( item == 10 );
        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 3 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 30 );
        REQUIRE( *(buffer.peekTail()) == 50 );
        REQUIRE( item == 20 );
        item = 60;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 30 );
        REQUIRE( *(buffer.peekTail()) == 60 );
        item = 70;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == true );
        REQUIRE( buffer.getNumItems() == 5 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 30 );
        REQUIRE( *(buffer.peekTail()) == 70 );


        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 4 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 40 );
        REQUIRE( *(buffer.peekTail()) == 70 );
        REQUIRE( item == 30 );
        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 3 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 50 );
        REQUIRE( *(buffer.peekTail()) == 70 );
        REQUIRE( item == 40 );
        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 2 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 60 );
        REQUIRE( *(buffer.peekTail()) == 70 );
        REQUIRE( item == 50 );
        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 70 );
        REQUIRE( *(buffer.peekTail()) == 70 );
        REQUIRE( item == 60 );
        REQUIRE( buffer.remove(item) == true );
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( item == 70 );


        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        
        item = 10;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 1 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 10 );
        item = 20;
        REQUIRE( buffer.add(item) == true );
        REQUIRE( buffer.isEmpty() == false );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.getNumItems() == 2 );
        REQUIRE( buffer.getMaxItems() == 5 );
        REQUIRE( *(buffer.peekHead()) == 10 );
        REQUIRE( *(buffer.peekTail()) == 20 );

        buffer.clearTheBuffer();
        REQUIRE( buffer.isEmpty() == true );
        REQUIRE( buffer.isFull() == false );
        REQUIRE( buffer.peekHead() == 0 );
        REQUIRE( buffer.peekTail() == 0 );
        REQUIRE( buffer.getNumItems() == 0 );
        REQUIRE( buffer.getMaxItems() == 5 );
        }

    
    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
    }
