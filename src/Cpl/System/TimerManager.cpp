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

#include "TimerManager.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/ElapsedTime.h"


///
using namespace Cpl::System;

#define SECT_   "Cpl::System"


///////////////////////////
TimerManager::TimerManager()
    :m_timeMark( 0 )
    , m_timeNow( 0 )
    , m_inTickCall( false )
{
}

void TimerManager::startManager( void ) noexcept
{
    m_timeMark = Cpl::System::ElapsedTime::milliseconds();
}

bool TimerManager::areActiveTimers( void ) noexcept
{
    return m_counters.first() == 0;
}

void TimerManager::processTimers( void ) noexcept
{
    // Calculate the elapsed time in milliseconds since we last checked the timers
    m_timeNow               = Cpl::System::ElapsedTime::milliseconds();
    unsigned long deltaTime = Cpl::System::ElapsedTime::deltaMilliseconds( m_timeMark, m_timeNow );

    // Update my timers
    CPL_SYSTEM_TRACE_MSG( SECT_, (" @@ START TICK: %lu, now=%lu, [m_timeMark=%lu]", deltaTime, m_timeNow, m_timeMark) );
    tick( deltaTime );
    m_timeMark = m_timeNow;
    CPL_SYSTEM_TRACE_MSG( SECT_, (" @@ TICK COMPLETE..., m_timeMark=%lu", m_timeMark) );
    tickComplete();
}


/////////////////////////
void TimerManager::tick( unsigned long msec ) noexcept
{
    // Set my state to: PROCESSING TICK(S)    
    m_inTickCall = true;

    while ( msec )
    {
        CounterCallback_* counterPtr = m_counters.first();
        if ( counterPtr == 0 )
        {
            // No timers registered -->do NOTHING
            break;
        }
        else
        {
            // Decrement the first/active counter in the list
            unsigned long nextCount = counterPtr->count();
            if ( msec <= nextCount )
            {
                counterPtr->decrement( msec );
                msec = 0;
            }
            else
            {
                counterPtr->decrement( nextCount );
                msec -= nextCount;
            }

            // Process ALL local Timers that have a ZERO countdown value
            while ( counterPtr && counterPtr->count() == 0 )
            {
                m_counters.get();                // Remove the expired counter from the list
                counterPtr->expired();           // Expire the counter
                counterPtr = m_counters.first(); // Get next counter
            }
        }
    }
}

void TimerManager::tickComplete( void ) noexcept
{
    // Process pending attaches now that the tick cycle has completed
    CounterCallback_* pendingClientPtr = m_pendingAttach.get();
    while ( pendingClientPtr )
    {
        addToActiveList( *pendingClientPtr );
        pendingClientPtr = m_pendingAttach.get();
    }

    // Clear my PROCESSING TICK(S) state
    m_inTickCall = false;
}


/////////////////////////
void TimerManager::attach( CounterCallback_& clientToCallback ) noexcept
{

    // Do NOT add to my active timer list while I am processing tick(s)!
    if ( m_inTickCall )
    {
        m_pendingAttach.put( clientToCallback );
    }

    // Add client timer
    else
    {
        addToActiveList( clientToCallback );
    }
}


void TimerManager::addToActiveList( CounterCallback_& clientToCallback ) noexcept
{
    // Insert the counter wisely into the list.  The counters are
    // stored in the list in the order they will expire and have their
    // raw counter value adjusted to be relative to any/all preceding list
    // elements.  This allows me to only decrement the first counter in 
    // the list - instead of all counters every tick.  
    CounterCallback_* counterPtr = m_counters.first();
    while ( counterPtr )
    {
        // Insert at the head of the list
        if ( clientToCallback.count() < counterPtr->count() )
        {
            counterPtr->decrement( clientToCallback.count() );
            m_counters.insertBefore( *counterPtr, clientToCallback );
            CPL_SYSTEM_TRACE_MSG( SECT_, (">> INSERT: %p, count=%lu, : BEFORE %p (%lu)", &clientToCallback, clientToCallback.count(), counterPtr, counterPtr->count()) );
            return;
        }

        // Insert in the middle
        clientToCallback.decrement( counterPtr->count() );
        if ( clientToCallback.count() == 0 )
        {
            m_counters.insertAfter( *counterPtr, clientToCallback );
            CPL_SYSTEM_TRACE_MSG( SECT_, (">> INSERT:: %p, count=%lu, AFTER %p (%lu)", &clientToCallback, clientToCallback.count(), counterPtr, counterPtr->count()) );
            return;
        }

        counterPtr = m_counters.next( *counterPtr );
    }

    // Insert at the tail (list is empty or largest counter value)
    CPL_SYSTEM_TRACE_MSG( SECT_, (">> INSERT @ end: %p, count=%lu", &clientToCallback, clientToCallback.count()) );
    m_counters.putLast( clientToCallback );
}


bool TimerManager::detach( CounterCallback_& clientToCallback ) noexcept
{
    // Try my pending list FIRST
    if ( m_pendingAttach.remove( clientToCallback ) )
    {
        return true;
    }

    // If I have the counter/timer -->it will be in the active list.  
    if ( m_counters.find( clientToCallback ) )
    {
        // Add the remaining time of the counter being remove to the next counter in the list
        CounterCallback_* nextPtr = m_counters.next( clientToCallback );
        if ( nextPtr )
        {
            nextPtr->increment( clientToCallback.count() );
        }

        // remove the counter
        m_counters.remove( clientToCallback );
        return true;
    }

    // If I get here, the Counter was NOT in the active list (AND it was not in the staging list)
    return false;
}


unsigned long TimerManager::msecToCounts( unsigned long milliseconds ) noexcept
{
    unsigned long delta = Cpl::System::ElapsedTime::deltaMilliseconds( m_timeNow );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("milliseconds IN=%lu, count out=%lu", milliseconds, milliseconds + delta) );
    return milliseconds + delta;
}
