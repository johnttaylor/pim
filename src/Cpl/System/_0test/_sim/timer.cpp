#if 0
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
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/_0test/timercommon.h"
#include "Cpl/System/SimTick.h"

using namespace Cpl::System;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "simtimer", "[simtimer]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create my test sequencers
    Master  fruits( SLOW_TICKSRC_MS_PER_TICK, DELAY_, "apple", APPLE_T1, APPLE_T2, "orange", ORANGE_T1, ORANGE_T2, Cpl::System::Thread::getCurrent() );
    Master  flowers( FAST_TICKSRC_MS_PER_TICK, DELAY_, "rose", ROSE_T1, ROSE_T2, "daisy", DAISY_T1, DAISY_T2, Cpl::System::Thread::getCurrent() );
    Master  trees( FAST_TICKSRC_MS_PER_TICK, DELAY_, "oak", OAK_T1, OAK_T2, "pine", PINE_T1, PINE_T2, Cpl::System::Thread::getCurrent() );

    // Create all of the threads
    Cpl::System::Thread* t1  = Cpl::System::Thread::create( fruits, "FRUITS" );
    Cpl::System::Thread* t2  = Cpl::System::Thread::create( trees, "TREES" );
    Cpl::System::Thread* t3  = Cpl::System::Thread::create( flowers, "FLOWERS" );
    REQUIRE( t1 );
    REQUIRE( t2 );
    REQUIRE( t3 );

    // Give time for all of threads to be created before starting the test sequence
    Cpl::System::Api::sleepInRealTime( 1000 );

    // Validate result of each sequence
    for ( int i=0; i < NUM_SEQ_; i++ )
    {
        // Start a test sequence
        fruits.notify( START_EVENT );
        flowers.notify( START_EVENT );
        trees.notify( START_EVENT );

        // Generate Simulated ticks
        size_t ticks = DELAY_ + DELAY_ / 2;
        while ( ticks )
        {
            size_t numTicks = ticks > DELAY_ / 8 ? DELAY_ / 8 : ticks;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Timer: advancing %lu ticks", numTicks ) );
            SimTick::advance( numTicks );
            Cpl::System::Api::sleepInRealTime( 100 );
            ticks -= numTicks;
        }

        // Wait for all event loops to complete a sequence
        Cpl::System::Thread::wait();
        Cpl::System::Thread::wait();
        Cpl::System::Thread::wait();
        Cpl::System::Api::sleepInRealTime( 300 );

        unsigned long maxCount = MAX_COUNT_( (unsigned long) fruits.m_appObj1.m_deltaTime1, APPLE_T1 );
        fruits.m_appObj1.displayTimer1( maxCount );
        REQUIRE( fruits.m_appObj1.m_count1 <= maxCount );
        REQUIRE( fruits.m_appObj1.m_minTime1 >= fruits.m_appObj1.m_timer1Duration );
        maxCount = MAX_COUNT_( (unsigned long) fruits.m_appObj1.m_deltaTime2, APPLE_T2 );
        fruits.m_appObj1.displayTimer2( maxCount );
        REQUIRE( fruits.m_appObj1.m_count2 <= maxCount );
        REQUIRE( fruits.m_appObj1.m_minTime2 >= fruits.m_appObj1.m_timer2Duration );

        maxCount = MAX_COUNT_( (unsigned long) fruits.m_appObj2.m_deltaTime1, ORANGE_T1 );
        fruits.m_appObj2.displayTimer1( maxCount );
        REQUIRE( fruits.m_appObj2.m_count1 <= maxCount );
        REQUIRE( fruits.m_appObj2.m_minTime1 >= fruits.m_appObj2.m_timer1Duration );
        maxCount = MAX_COUNT_( (unsigned long) fruits.m_appObj2.m_deltaTime2, ORANGE_T2 );
        fruits.m_appObj2.displayTimer2( maxCount );
        REQUIRE( fruits.m_appObj2.m_count2 <= maxCount );
        REQUIRE( fruits.m_appObj2.m_minTime2 >= fruits.m_appObj2.m_timer2Duration );

        maxCount = MAX_COUNT_( (unsigned long) trees.m_appObj1.m_deltaTime1, OAK_T1 );
        trees.m_appObj1.displayTimer1( maxCount );
        REQUIRE( trees.m_appObj1.m_count1 <= maxCount );
        REQUIRE( trees.m_appObj1.m_minTime1 >= trees.m_appObj1.m_timer1Duration );
        maxCount = MAX_COUNT_( (unsigned long) trees.m_appObj1.m_deltaTime2, OAK_T2 );
        trees.m_appObj1.displayTimer2( maxCount );
        REQUIRE( trees.m_appObj1.m_count2 <= maxCount );
        REQUIRE( trees.m_appObj1.m_minTime2 >= trees.m_appObj1.m_timer2Duration );

        maxCount = MAX_COUNT_( (unsigned long) trees.m_appObj2.m_deltaTime1, PINE_T1 );
        trees.m_appObj2.displayTimer1( maxCount );
        REQUIRE( trees.m_appObj2.m_count1 <= maxCount );
        REQUIRE( trees.m_appObj2.m_minTime1 >= trees.m_appObj2.m_timer1Duration );
        maxCount = MAX_COUNT_( (unsigned long) trees.m_appObj2.m_deltaTime2, PINE_T2 );
        trees.m_appObj2.displayTimer2( maxCount );
        REQUIRE( trees.m_appObj2.m_count2 <= maxCount );
        REQUIRE( trees.m_appObj2.m_minTime2 >= trees.m_appObj2.m_timer2Duration );

        maxCount = MAX_COUNT_( (unsigned long) flowers.m_appObj1.m_deltaTime1, ROSE_T1 );
        flowers.m_appObj1.displayTimer1( maxCount );
        REQUIRE( flowers.m_appObj1.m_count1 <= maxCount );
        REQUIRE( flowers.m_appObj1.m_minTime1 >= flowers.m_appObj1.m_timer1Duration );
        maxCount = MAX_COUNT_( (unsigned long) flowers.m_appObj1.m_deltaTime2, ROSE_T2 );
        flowers.m_appObj1.displayTimer2( maxCount );
        REQUIRE( flowers.m_appObj1.m_count2 <= maxCount );
        REQUIRE( flowers.m_appObj1.m_minTime2 >= flowers.m_appObj1.m_timer2Duration );

        maxCount = MAX_COUNT_( (unsigned long) flowers.m_appObj2.m_deltaTime1, DAISY_T1 );
        flowers.m_appObj2.displayTimer1( maxCount );
        REQUIRE( flowers.m_appObj2.m_count1 <= maxCount );
        REQUIRE( flowers.m_appObj2.m_minTime1 >= flowers.m_appObj2.m_timer1Duration );
        maxCount = MAX_COUNT_( (unsigned long) flowers.m_appObj2.m_deltaTime2, DAISY_T2 );
        flowers.m_appObj2.displayTimer2( maxCount );
        REQUIRE( flowers.m_appObj2.m_count2 <= maxCount );
        REQUIRE( flowers.m_appObj2.m_minTime2 >= flowers.m_appObj2.m_timer2Duration );
    }


    // Shutdown threads
    fruits.pleaseStop();
    trees.pleaseStop();
    flowers.pleaseStop();

    // Generate Simulated ticks
    SimTick::advance( DELAY_ );

    Cpl::System::Api::sleepInRealTime( 300 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    REQUIRE( t3->isRunning() == false );

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    Cpl::System::Thread::destroy( *t3 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
#endif