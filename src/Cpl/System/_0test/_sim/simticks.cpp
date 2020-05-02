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
#include "Cpl/System/Thread.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////
namespace {

class Master : public Runnable
{
public:
    ///
    Semaphore & m_sema;
    ///
    Thread*     m_appleThreadPtr;
    ///
    Thread*     m_cherryThreadPtr;
    ///
    Thread*     m_myThreadPtr;

public:
    ///
    Master( Semaphore&  sema )
        :m_sema( sema ),
        m_appleThreadPtr( 0 ),
        m_cherryThreadPtr( 0 ),
        m_myThreadPtr( 0 )
    {
    }

public:
    ///
    void appRun()
    {
        // Wait for the test to be started
        Thread::wait();

        // TEST#1 Round-robing signaling of thread
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaling: %s", m_appleThreadPtr->getName()) );
        m_appleThreadPtr->signal();

        // Test#1 Round Robin threads
        int i;
        for ( i=0; i < 3 + 3 + 3; i++ )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Wait %d", i + 1) );
            Thread::wait();
        }

        // Test#2 Timed semaphores
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Test#2 (Timed semaphores - wait to start)") );
        Thread::wait();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Test#2 (Timed semaphores - starting)") );
        m_sema.signal();
        m_sema.signal();
        m_sema.signal();
        Thread::wait();

        // Test#3 Timed Thread waits
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Test#3 (Timed Thread waits - Part1 )") );
        m_cherryThreadPtr->signal();
        Thread::wait();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Test#3 (Timed Thread waits - Part2)") );
        m_cherryThreadPtr->signal();
        m_cherryThreadPtr->signal();
        Thread::wait();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Test#3 (Timed Thread waits - DONE)") );
    }


    /// Cache my thread pointer
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr )
    {
        m_myThreadPtr = myThreadPtr;
    }

    /// Override default implementation
    void pleaseStop( void )
    {
        if ( isRunning() && m_myThreadPtr )
        {
            m_myThreadPtr->signal();
        }
    }

};

class MyRunnable : public Runnable
{
public:
    ///
    Thread & m_masterThread;
    ///
    Thread*     m_nextThreadPtr;
    ///
    int         m_loops;
    ///
    int         m_maxLoops;
    ///
    Thread*     m_myThreadPtr;

public:
    ///
    MyRunnable( Thread& masterThread, Thread* nextThreadPtr, int maxLoops )
        :m_masterThread( masterThread ),
        m_nextThreadPtr( nextThreadPtr ),
        m_loops( 0 ),
        m_maxLoops( maxLoops ),
        m_myThreadPtr( 0 )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        // Wait for the test to be started
        Thread::wait();

        while ( isRunning() && m_loops < m_maxLoops )
        {
            m_loops++;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Waiting.... (loops=%d)", m_loops) );
            Thread::wait();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaled.  (loops=%d)", m_loops) );
            if ( isRunning() && m_nextThreadPtr )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaling: %s", m_nextThreadPtr->getName()) );
                m_nextThreadPtr->signal();
            }

            m_masterThread.signal();
        }
    }


    /// Cache my thread pointer
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr )
    {
        m_myThreadPtr = myThreadPtr;
    }

    /// Override default implementation
    void pleaseStop( void )
    {
        if ( isRunning() && m_myThreadPtr )
        {
            m_myThreadPtr->signal();
        }
    }

};

class MyRunnable2 : public Runnable
{
public:
    ///
    Thread & m_masterThread;
    ///
    Semaphore&    m_sema;
    ///
    bool          m_waitResult1;
    ///
    bool          m_waitResult2;
    ///
    unsigned long m_delta1;
    ///
    unsigned long m_delta2;
    ///
    unsigned long m_delta3;
    ///
    unsigned long m_delta4;
    ///
    unsigned long m_delta5;

public:
    ///
    MyRunnable2( Thread& masterThread, Semaphore& semaphore )
        :m_masterThread( masterThread ),
        m_sema( semaphore )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Semaphore Time wait test1 (timeout occurs)...") );
        unsigned long start = ElapsedTime::milliseconds();
        m_sema.timedWait( 333 );
        m_waitResult1 = m_sema.tryWait();                 // Should be false;
        m_delta1      = ElapsedTime::deltaMilliseconds( start );      // Should be >= 333
        m_masterThread.signal();

        // What till the the test harness is ready for the next test
        m_sema.wait();
        Api::sleep( 30 ); // Allow time for the semaphore to signaled more than once.

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Semaphore Time wait test2 (no timeout)...") );
        start = ElapsedTime::milliseconds();
        m_waitResult2 = m_sema.tryWait();                 // Should be true;
        m_sema.timedWait( 1000 );
        m_delta2      = ElapsedTime::deltaMilliseconds( start );      // Should be close to zero
        m_masterThread.signal();

        // What till the the test harness is ready for the next test
        Thread::wait();
        Api::sleep( 1 ); // Allow time for the semaphore to signaled more than once.

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Thread Time wait test1 (timeout occurs)...") );
        start     = ElapsedTime::milliseconds();
        Thread::timedWait( 333 );
        m_delta4  = ElapsedTime::deltaMilliseconds( start );      // Should be >= 333
        m_masterThread.signal();

        // What till the the test harness is ready for the next test
        Thread::wait();
        Api::sleep( 1 );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Thread Time wait test2 (no timeout)...") );
        start    = ElapsedTime::milliseconds();
        Thread::timedWait( 1000 );
        m_delta5 = ElapsedTime::deltaMilliseconds( start );      // Should be close to zero
        m_masterThread.signal();


        CPL_SYSTEM_TRACE_MSG( SECT_, ("Results: wait1=%d =? 0, delta1=%d >=? 333, wait2=%d =? 1, delta2=%d <? 50, delta4=%d >=? 333, delta5=%d <? 50.", m_waitResult1, (unsigned) m_delta1, m_waitResult2, (unsigned) m_delta2, (unsigned) m_delta4, (unsigned) m_delta5) );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Sleeping (40)...") );
        start    = ElapsedTime::milliseconds();
        Api::sleep( 40 );
        m_delta3 = ElapsedTime::deltaMilliseconds( start );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DONE Sleeping (delta=%d)", (unsigned) m_delta3) );
    }
};

class MyRunnable3 : public Runnable
{
public:
    volatile bool m_run;

public:
    ///
    MyRunnable3()
        :m_run( true )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        while ( m_run )
        {
            Cpl::System::Api::sleep( 1 );
        }
    }


    /// Override default implementation
    void pleaseStop( void )
    {
        m_run = false;
    }

};

class Lister : public Thread::Traverser
{
public:
    ///
    bool m_foundApple;
    bool m_foundOrange;
    bool m_foundPear;

public:
    ///
    Lister()
        :m_foundApple( false ),
        m_foundOrange( false ),
        m_foundPear( false )
    {
    }
public:
    Cpl::Type::Traverser::Status_T item( Thread& nextThread )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("<%-10s %5lu>", nextThread.getName(), nextThread.getId()) );

        if ( strcmp( nextThread.getName(), "Apple" ) == 0 )
        {
            m_foundApple = true;
        }
        else if ( strcmp( nextThread.getName(), "Orange" ) == 0 )
        {
            m_foundOrange = true;
        }
        else if ( strcmp( nextThread.getName(), "Pear" ) == 0 )
        {
            m_foundPear = true;
        }


        return Cpl::Type::Traverser::eCONTINUE;
    }
};


}; // end namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "simbasic", "[simbasic]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();

    CPL_SYSTEM_TRACE_MSG( SECT_, ("Creating Test Threads.  Real Elapsed time=%lu", ElapsedTime::millisecondsInRealTime()) );

    Semaphore sema;
    Master    masterRun( sema );
    Thread*   masterThreadPtr   = Thread::create( masterRun, "MASTER", CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER );

    MyRunnable appleRun( *masterThreadPtr, 0, 3 );
    Thread* appleThreadPtr      = Thread::create( appleRun, "Apple", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

    MyRunnable orangeRun( *masterThreadPtr, appleThreadPtr, 3 );
    Thread* orangeThreadPtr  = Thread::create( orangeRun, "Orange", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

    MyRunnable pearRun( *masterThreadPtr, orangeThreadPtr, 3 );
    Thread* pearThreadPtr    = Thread::create( pearRun, "Pear", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
    appleRun.m_nextThreadPtr = pearThreadPtr;

    MyRunnable2 cherryRun( *masterThreadPtr, sema );
    Thread* cherryThreadPtr   = Thread::create( cherryRun, "Cherry", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

    MyRunnable3 timerRun;
    Thread* timerThreadPtr    = Thread::create( timerRun, "Timer", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );


    // Update my master thread with additional info
    masterRun.m_appleThreadPtr  = appleThreadPtr;
    masterRun.m_cherryThreadPtr = cherryThreadPtr;

    Lister myThreadList;
    Cpl::System::Api::sleepInRealTime( 500 );    // Give time to ensure all threads are active.
    CPL_SYSTEM_TRACE_MSG( SECT_, ("ALL Threads should have been created and up and running.  Real Elapsed time=%lu", ElapsedTime::millisecondsInRealTime()) );
    Thread::traverse( myThreadList );
    REQUIRE( myThreadList.m_foundApple );
    REQUIRE( myThreadList.m_foundOrange );
    REQUIRE( myThreadList.m_foundPear );

    // Start the tests
    masterThreadPtr->signal();
    appleThreadPtr->signal();
    pearThreadPtr->signal();
    orangeThreadPtr->signal();

    // Advance simulated time for the FIRST test
    while ( appleThreadPtr->isRunning() ||
            orangeThreadPtr->isRunning() ||
            pearThreadPtr->isRunning() )
    {
        if ( !SimTick::advance( 1 ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("EXITED tick loop early (waiting on apple, orange, & pear)") );
            break;
        }
    }


    CPL_SYSTEM_TRACE_MSG( SECT_, ("Cleaning up Pear, Apple, and Orange... [SimTime=%lu]", (unsigned long) SimTick::current()) );
    Thread::destroy( *orangeThreadPtr );
    Thread::destroy( *pearThreadPtr );
    Thread::destroy( *appleThreadPtr );

    // Advance simulated time for the SECOND test (and to finish the terminate of the orange thread)
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Waiting for timed Semaphore test to complete") );
    while ( cherryThreadPtr->isRunning() ||
            masterThreadPtr->isRunning() )
    {
        if ( !SimTick::advance( 1 ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("EXITED tick loop early (waiting on cherry & master)") );
            break;
        }
    }

    // Shutdown timer thread
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Shutting down timer thread...") );
    timerRun.pleaseStop();
    while ( timerThreadPtr->isRunning() )
    {
        if ( !SimTick::advance( 1 ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("EXITED tick loop early (waiting on timer)") );
            break;
        }
    }

    // Clean-up
    CPL_SYSTEM_TRACE_MSG( SECT_, ("ALL simulated ticks completed.  [SimTime=%lu]", (unsigned long) SimTick::current()) );
    Thread::destroy( *cherryThreadPtr );
    Thread::destroy( *masterThreadPtr );
    Thread::destroy( *timerThreadPtr );

    REQUIRE( appleRun.m_loops == 3 );
    REQUIRE( orangeRun.m_loops == 3 );
    REQUIRE( pearRun.m_loops == 3 );


    REQUIRE( cherryRun.m_waitResult1 == false );
    REQUIRE( cherryRun.m_delta1 >= 333 );
    REQUIRE( cherryRun.m_waitResult2 == true );
    REQUIRE( cherryRun.m_delta2 < 50 );
    REQUIRE( cherryRun.m_delta3 >= 40 );
    REQUIRE( cherryRun.m_delta4 >= 333 );
    REQUIRE( cherryRun.m_delta5 < 50 );

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
