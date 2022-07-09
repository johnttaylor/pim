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
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/EventLoop.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////
namespace {

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
    ///
    Tls&        m_tls;
    ///
    const char* m_expectedTlsValue;
    ///
    int         m_tlsCompareResult;

public:
    ///
    MyRunnable( Thread& masterThread, Thread* nextThreadPtr, int maxLoops, Tls& tlsVariable, const char* expectedTlsValue )
        :m_masterThread( masterThread ),
        m_nextThreadPtr( nextThreadPtr ),
        m_loops( 0 ),
        m_maxLoops( maxLoops ),
        m_myThreadPtr( 0 ),
        m_tls( tlsVariable ),
        m_expectedTlsValue( expectedTlsValue ),
        m_tlsCompareResult( -1 )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        m_tlsCompareResult = strcmp( (const char*) m_tls.get(), m_expectedTlsValue );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("TLS Compare: '%s' =? '%s', result=%d", (const char*) m_tls.get(), m_expectedTlsValue, m_tlsCompareResult) );

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
        m_tls.set( (void*) m_myThreadPtr->getName() );
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
    Thread*       m_myThreadPtr;
    ///
    Tls&          m_tls;
    ///
    const char*   m_expectedTlsValue;
    ///
    int           m_tlsCompareResult;
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

public:
    ///
    MyRunnable2( Thread& masterThread, Semaphore& semaphore, Tls& tlsVariable, const char* expectedTlsValue )
        :m_masterThread( masterThread ),
        m_sema( semaphore ),
        m_myThreadPtr( 0 ),
        m_tls( tlsVariable ),
        m_expectedTlsValue( expectedTlsValue ),
        m_tlsCompareResult( -1 )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        m_tlsCompareResult = strcmp( (const char*) m_tls.get(), m_expectedTlsValue );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("TLS Compare: '%s' =? '%s', result=%d", (const char*) m_tls.get(), m_expectedTlsValue, m_tlsCompareResult) );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Semaphore Time wait test1 (timeout occurs)...") );
        unsigned long start = ElapsedTime::milliseconds();
        m_sema.timedWait( 333 );
        m_waitResult1 = m_sema.tryWait();                 // Should be false;
        m_delta1      = ElapsedTime::deltaMilliseconds( start );      // Should be >= 333
        m_masterThread.signal();
        Api::sleep( 100 );         // Allow time for my test harness to run, i.e. increment the semaphore for the next test

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Semaphore Time wait test2 (no timeout)...") );
        start = ElapsedTime::milliseconds();
        m_waitResult2 = m_sema.tryWait();                 // Should be true;
        m_sema.timedWait( 1000 );
        m_delta2      = ElapsedTime::deltaMilliseconds( start );      // Should be close to zero
        m_masterThread.signal();

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Thread Time wait test1 (timeout occurs)...") );
        start    = ElapsedTime::milliseconds();
        Thread::timedWait( 333 );
        m_delta3 = ElapsedTime::deltaMilliseconds( start );      // Should be >= 333
        m_masterThread.signal();
        Api::sleep( 100 );         // Allow time for my test harness to run, i.e. increment the semaphore for the next test

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Thread Time wait test2 (no timeout)...") );
        start    = ElapsedTime::milliseconds();
        Thread::timedWait( 1000 );
        m_delta4 = ElapsedTime::deltaMilliseconds( start );      // Should be close to zero
        m_masterThread.signal();

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Results: wait1=%d =? 0, delta1=%d >? 333, wait2=%d =? 1, delta2=%d <? 50, delta3=%d >? 333, delta4=%d <? 50.", m_waitResult1, (unsigned) m_delta1, m_waitResult2, (unsigned) m_delta2, (unsigned) m_delta3, (unsigned) m_delta4) );
    }


    /// Cache my thread pointer
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr )
    {
        m_myThreadPtr = myThreadPtr;
        m_tls.set( (void*) m_myThreadPtr->getName() );
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
static int counter01 = 0;
static int counter02 = 0;
static int counter03 = 2;
static int counter11 = 0;
static int counter13 = 2;
static int counter21 = 0;

static void callback1( void* context )
{
    int* ptr = (int*) context;
    *ptr = *ptr + 1;
}
static void callback2( void* context )
{
    int* ptr = (int*) context;
    *ptr = *ptr - 1;
}
static void callback3( void* context )
{
    int* ptr = (int*) context;
    *ptr = *ptr + 10;
}

static Cpl::System::SharedEventHandlerApi::EventCallback_T callbacks1[3] ={ {callback1, &counter01}, {callback2, &counter02}, {callback3, &counter03} };
static Cpl::System::SharedEventHandlerApi::EventCallback_T callbacks2[3] ={ {callback1, &counter11}, {0,0}, { callback3, &counter13 } };
static Cpl::System::SharedEventHandlerApi::EventCallback_T callbacks3[1] ={ {callback1, &counter21} };


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "basic" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();

    SECTION( "main test" )
    {
        Tls runnableName;

        MyRunnable appleRun( Thread::getCurrent(), 0, 3, runnableName, "Apple" );
        Thread* appleThreadPtr   = Thread::create( appleRun, "Apple" );

        MyRunnable orangeRun( Thread::getCurrent(), appleThreadPtr, 4, runnableName, "Orange" );
        Thread* orangeThreadPtr  = Thread::create( orangeRun, "Orange" );

        MyRunnable pearRun( Thread::getCurrent(), orangeThreadPtr, 3, runnableName, "Pear" );
        Thread* pearThreadPtr    = Thread::create( pearRun, "Pear" );


        Lister myThreadList;
        Cpl::System::Api::sleep( 100 );    // Give time to ensure all threads are active.
        Thread::traverse( myThreadList );
        REQUIRE( myThreadList.m_foundApple );
        REQUIRE( myThreadList.m_foundOrange );
        REQUIRE( myThreadList.m_foundPear );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("Signaling: %s", appleThreadPtr->getName()) );
        appleRun.m_nextThreadPtr = pearThreadPtr;
        appleThreadPtr->signal();

        int i;
        for ( i=0; i < 3 + 3 + 3; i++ )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Wait %d", i + 1) );
            Thread::wait();
        }

        Thread::destroy( *pearThreadPtr );
        REQUIRE( orangeThreadPtr->isRunning() );
        Thread::destroy( *orangeThreadPtr ); // Note: this is still an active thread at this point
        Thread::destroy( *appleThreadPtr );

        REQUIRE( appleRun.m_tlsCompareResult == 0 );
        REQUIRE( orangeRun.m_tlsCompareResult == 0 );
        REQUIRE( pearRun.m_tlsCompareResult == 0 );

        REQUIRE( appleRun.m_loops == 3 );
        REQUIRE( orangeRun.m_loops == 4 );
        REQUIRE( pearRun.m_loops == 3 );
        Thread::wait();
        REQUIRE( Thread::tryWait() == false );

        Semaphore sema;
        MyRunnable2 cherryRun( Thread::getCurrent(), sema, runnableName, "Cherry" );
        Thread* cherryThreadPtr   = Thread::create( cherryRun, "Cherry" );
        Thread::wait();
        sema.signal();
        sema.signal();
        Thread::wait();
        Thread::wait();
        cherryThreadPtr->signal();
        Thread::wait();
        REQUIRE( cherryRun.m_tlsCompareResult == 0 );
        REQUIRE( cherryRun.m_waitResult1 == false );
        REQUIRE( cherryRun.m_delta1 >= 333 - 2 );        // Use a tolerance for the test since the elapsed time and timed semaphore are guaranteed to have the same timing source
        REQUIRE( cherryRun.m_waitResult2 == true );
        REQUIRE( cherryRun.m_delta2 < 50 );
        REQUIRE( cherryRun.m_delta3 >= 333 - 2 );
        REQUIRE( cherryRun.m_delta4 < 50 );

        Api::sleep( 50 ); // Allow time for the Cherry thread to self terminate
    }

    SECTION( "Semaphores" )
    {
        Thread::getCurrent().signal();
        Thread::wait();
        bool result = Thread::tryWait();
        REQUIRE( result == false );
        Thread::getCurrent().signal();
        result = Thread::tryWait();
        REQUIRE( result == true );
        Thread::getCurrent().signal();
        result = Thread::timedWait( 100 );
        REQUIRE( result == true );
        result = Thread::timedWait( 100 );
        REQUIRE( result == false );
    }

    SECTION("SuspendScheduling")
    {
        // Cant really test on a desktop OS - but at least we can make sure that there is an implementation for (i.e. verify compile & link)
        Api::suspendScheduling();
        Api::resumeScheduling();
    }

#define NUM_SEQ_    3
#define EVENT_FLAGS 4

    SECTION( "events" )
    {
        Cpl::System::SharedEventHandler<3> eventHandler1( callbacks1 );
        Cpl::System::SharedEventHandler<3> eventHandler2( callbacks2 );
        Cpl::System::SharedEventHandler<1> eventHandler3( callbacks3 );

        Cpl::System::EventLoop fruits( OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventHandler1 );
        Cpl::System::EventLoop trees( OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventHandler2 );
        Cpl::System::EventLoop flowers( OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventHandler3 );

        // Create all of the threads
        Cpl::System::Thread* t1  = Cpl::System::Thread::create( fruits, "FRUITS" );
        Cpl::System::Thread* t2  = Cpl::System::Thread::create( trees, "TREES" );
        Cpl::System::Thread* t3  = Cpl::System::Thread::create( flowers, "FLOWERS" );

        // Give time for all of threads to be created before starting the test sequence
        Cpl::System::Api::sleep( 100 );

        // Run the sequence N times
        for ( int j=1; j <= NUM_SEQ_; j++ )
        {
            // Signal the event flags
            for ( int i=0; i < EVENT_FLAGS; i++ )
            {
                // Start a test sequence
                fruits.notify( i );
                flowers.notify( i );
                trees.notify( i );
                Cpl::System::Api::sleep( 100 );  // Allow other threads to process
            }

            REQUIRE( counter01 == j );
            REQUIRE( counter02 == -j );
            REQUIRE( counter03 == j * 10 + 2 );
            REQUIRE( counter11 == j );
            REQUIRE( counter13 == j * 10 + 2 );
            REQUIRE( counter21 == j );
        }

        // Shutdown threads
        fruits.pleaseStop();
        trees.pleaseStop();
        flowers.pleaseStop();

        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
        REQUIRE( t1->isRunning() == false );
        REQUIRE( t2->isRunning() == false );
        REQUIRE( t3->isRunning() == false );

        Cpl::System::Thread::destroy( *t1 );
        Cpl::System::Thread::destroy( *t2 );
        Cpl::System::Thread::destroy( *t3 );
        Cpl::System::Api::sleep( 300 ); // allow time for threads to stop
    }
    
    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
