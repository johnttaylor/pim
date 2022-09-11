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

#include "EventLoop.h"
#include "SimTick.h"
#include "Trace.h"
#include "FatalError.h"
#include "GlobalLock.h"
#include "ElapsedTime.h"

#define SECT_ "Cpl::System"


///
using namespace Cpl::System;

/////////////////////
EventLoop::EventLoop( unsigned long          timeOutPeriodInMsec,
                      SharedEventHandlerApi* eventHandler )
    : m_myThreadPtr( 0 )
    , m_eventHandler( eventHandler )
    , m_sema()
    , m_timeout( timeOutPeriodInMsec )
    , m_timeStartOfLoop( 0 )
    , m_events( 0 )
    , m_run( true )
{
    if ( timeOutPeriodInMsec == 0 )
    {
        FatalError::logf( "EventLoop(%p): timeOutPeriodInMsec can NOT be set to zero", this );
    }
}
void EventLoop::setThreadOfExecution_( Thread* myThreadPtr )
{
    m_myThreadPtr = myThreadPtr;
}

int EventLoop::signal( void ) noexcept
{
    return m_sema.signal();
}

int EventLoop::su_signal( void ) noexcept
{
    return m_sema.su_signal();
}

void EventLoop::appRun( void )
{
    startEventLoop();
    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
    }
    stopEventLoop();
}

void EventLoop::startEventLoop() noexcept
{
    // Initialize/start the timer manager
    startManager();
}

void EventLoop::stopEventLoop() noexcept
{
    // Nothing currently needed
}

void EventLoop::processEventFlag( uint8_t eventNumber ) noexcept
{
    if ( m_eventHandler )
    {
        m_eventHandler->processEventFlag( eventNumber );
    }
}

bool EventLoop::waitAndProcessEvents( bool skipWait ) noexcept
{
    // Trap my exit/please-stop condition
    GlobalLock::begin();
    bool stayRunning = m_run;
    GlobalLock::end();
    if ( !stayRunning )
    {
        return false;
    }

    // Skip waiting if it has been a 'long time' since we last processed events
    unsigned long now = ElapsedTime::milliseconds();
    if ( ElapsedTime::deltaMilliseconds( m_timeStartOfLoop, now ) > m_timeout )
    {
        skipWait = true;
    }
    m_timeStartOfLoop = now;

    // Wait for something to happen...
    if ( !skipWait )
    {
        m_sema.timedWait( m_timeout ); // Note: For Tick Simulation: the timedWait() calls topLevelWait() if the semaphore has not been signaled
    }

    // Trap my exit/please-stop condition AGAIN since a lot could have happen while I was waiting....
    GlobalLock::begin();
    stayRunning = m_run;
    GlobalLock::end();
    if ( !stayRunning )
    {
        return false;
    }

    // Capture the current state of the event flags
    GlobalLock::begin();
    Cpl_System_EventFlag_T events = m_events;
    m_events                      = 0;
    GlobalLock::end();

    // Process Event Flags
    if ( events )
    {
        Cpl_System_EventFlag_T eventMask   = 1;
        uint8_t                eventNumber = 0;
        for ( ; eventMask; eventMask <<= 1, eventNumber++ )
        {
            if ( (events & eventMask) )
            {
                processEventFlag( eventNumber );
            }
        }
    }

    // Timer Check
    processTimers();
    return true;
}

/////////////////////
void EventLoop::pleaseStop()
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    // Set my flag/state to exit my top level thread loop
    GlobalLock::begin();
    m_run = false;
    GlobalLock::end();

    // Signal myself in case the thread is blocked waiting for the 'next event'
    m_sema.signal();
}

////////////////////////////////////////////////////////////////////////////////
void EventLoop::notifyEvents( Cpl_System_EventFlag_T events ) noexcept
{
    // Mark that I was signaled
    GlobalLock::begin();
    m_events |= events;
    GlobalLock::end();

    m_sema.signal();
}

void EventLoop::notify( uint8_t eventNumber ) noexcept
{
    notifyEvents( 1 << eventNumber );
}

// NOTE: Same logic as notifyEvents(), EXCEPT no critical section is used -->this is because su_signal() is called from an ISR and no mutex is required (and mutexes don't work in from ISRs anyway)
void EventLoop::su_notifyEvents( Cpl_System_EventFlag_T events ) noexcept
{
    // Mark that I was signaled 
    m_events    |= events;
    m_sema.su_signal();
}

void EventLoop::su_notify( uint8_t eventNumber ) noexcept
{
    su_notifyEvents( 1 << eventNumber );
}

