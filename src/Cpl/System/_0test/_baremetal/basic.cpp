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
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/EventLoop.h"
#include "Cpl/System/Timer.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/BareMetal/Thread.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>
#include <stdint.h>
#include <math.h>

#define SECT_     "_0test"
/// 
using namespace Cpl::System;


class EventFlagProcessing : public EventLoop
{
public:
    int                     m_eventCount;
    Cpl_System_EventFlag_T  m_eventsReceived;
    uint8_t                 m_exitEventNum;

public:
    ///
    EventFlagProcessing( uint8_t exitEventNum ):m_eventCount( 0 ), m_eventsReceived( 0 ), m_exitEventNum( exitEventNum ) {};

    ///
    void processEventFlag( uint8_t eventNumber ) noexcept
    {
        m_eventCount++;
        m_eventsReceived |= 1 << eventNumber;
        if ( eventNumber == m_exitEventNum )
        {
            pleaseStop();
        }
        CPL_SYSTEM_TRACE_MSG( SECT_, ("<%-10s>. processEventFlag. event=%d, count=%d, rx=%0lX", Thread::myName(), eventNumber, m_eventCount, m_eventsReceived) );
    }
};

class TestTimer : public Timer
{
public:
    const char* m_name;
    unsigned long m_duration;
    int m_count;
    int m_exitCount;

    ///
    TestTimer( TimerManager& timingSource, const char* name, unsigned long duration, int exitCount ):Timer( timingSource ), m_name( name ), m_duration( duration ), m_count( 0 ), m_exitCount( exitCount ) {}

    void expired( void ) noexcept
    {
        m_count++;
        int abs_exitCount = abs( m_exitCount );
        if ( m_count < abs_exitCount )
        {
            start( m_duration );
        }
        else if ( m_exitCount > 0 )
        {
            Thread::myRunnable().pleaseStop();
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("<%-10s>. TestTimer::Expired(%s). count=%d", Thread::myName(), m_name, m_count) );
    }
};

class Lister : public Thread::Traverser
{
public:
    ///
    int m_count;

public:
    ///
    Lister()
        :m_count( 0 )
    {
    }
public:
    Cpl::Type::Traverser::Status_T item( Thread& nextThread )
    {
        m_count++;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("<%-10s %5lu>. current count=%d", nextThread.getName(), nextThread.getId(), m_count) );
        return Cpl::Type::Traverser::eCONTINUE;
    }
};



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "single thread" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();

    SECTION( "List threads" )
    {
        Lister myThreadList;
        Thread::traverse( myThreadList );
        REQUIRE( myThreadList.m_count == 1 );
    }

    SECTION( "EventFlags" )
    {
        EventFlagProcessing eventLoop( 31 );
        Runnable& prev = BareMetal::Thread::setRunnable( eventLoop );

        eventLoop.notify( 1 );
        eventLoop.notify( 3 );
        eventLoop.notify( 5 );
        eventLoop.notify( 31 );
        eventLoop.run();
        REQUIRE( eventLoop.m_eventCount == 4 );
        REQUIRE( eventLoop.m_eventsReceived == 0x8000002A );

        BareMetal::Thread::setRunnable( prev );
    }

    SECTION( "Timers" )
    {
        EventFlagProcessing eventLoop( 0 );
        Runnable& prev = BareMetal::Thread::setRunnable( eventLoop );

        TestTimer t1( eventLoop, "t1", 1000, 3 );
        TestTimer t2( eventLoop, "t2", 10, -10 );
        TestTimer t3( eventLoop, "t3", 300, -3 );

        t1.start( 1000 );
        t2.start( 10 );
        t3.start( 300 );

        eventLoop.run();
        REQUIRE( t1.m_count == 3 );
        REQUIRE( t2.m_count == 10 );
        REQUIRE( t3.m_count == 3 );

        BareMetal::Thread::setRunnable( prev );
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

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
