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

#include "Cpl/System/SimTick.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Container/SList.h"
#include <new>

#include <stdio.h>

/// 
using namespace Cpl::System;

static Tls*                           simTlsPtr_      = 0;
static size_t                         milliseconds_   = 0;
static size_t                         remainingTicks_ = 0;
static unsigned long                  seconds_        = 0;
static Semaphore                      tickSource_;
static Mutex                          myLock_;
static Cpl::Container::SList<SimTick> waiters_;



/////////////////////////////////////////
unsigned SimTick::wakeUpWaiters( void ) noexcept
{
    unsigned waiters    = 0;
    SimTick* simInfoPtr = waiters_.get();
    while ( simInfoPtr )
    {
        waiters++;
        simInfoPtr->m_ackPending  = true;
        simInfoPtr->m_waiter.signal();
        simInfoPtr = waiters_.get();
    }
    return waiters;
}

unsigned SimTick::getCurrentWaitersCount( void ) noexcept
{
    unsigned waiters    = 0;
    SimTick* simInfoPtr = waiters_.first();
    while ( simInfoPtr )
    {
        waiters++;
        simInfoPtr = waiters_.next( *simInfoPtr );
    }
    return waiters;
}

bool SimTick::advance( size_t numTicks ) noexcept
{
    bool result = true;
    numTicks += remainingTicks_;

    while ( numTicks >= OPTION_CPL_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE )
    {
        numTicks -= OPTION_CPL_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE;

        // Drain any 'left-over/initial' sim-thread-ack-signals
        while ( tickSource_.tryWait() )
            ;

        // START critical section
        myLock_.lock();

        // Increment my system time
        milliseconds_ += OPTION_CPL_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE;
        seconds_       = milliseconds_ / 1000;

        // Wake-up all of threads waiting on a simulate tick
        unsigned waiters = wakeUpWaiters();

        // END critical section
        myLock_.unlock();

        // Wait for all the 'sim threads' to complete their tick processing
        while ( waiters-- )
        {
            tickSource_.wait();
        }
    }

    remainingTicks_ = numTicks;
    return result;
}


void SimTick::applicationWait( void ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();

    // NEVER block if a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return;
    }

    // Notify the tick source that I have complete my processing for the system tick (this blocking wait aborts remaining thread iterations)
    if ( simInfoPtr->m_ackPending )
    {
        simInfoPtr->m_ackPending = false;
        tickSource_.signal();
    }
}



void SimTick::topLevelWait( void ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();

    // NEVER block if a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return;
    }


    // Test my thread's tick counter against the System tick count and queue my thread to wait for next tick if tick counts match
    if ( testAndQueue( simInfoPtr ) )
    {
        simInfoPtr->m_ackPending = false;
        tickSource_.signal();
        simInfoPtr->m_waiter.waitInRealTime();
    }
}


bool SimTick::testAndQueue( SimTick* simInfoPtr ) noexcept
{
    // SCOPE Critical section
    Cpl::System::Mutex::ScopeBlock lock( myLock_ );

    // Wait while my thread's simulate time match the system simulate time
    if ( simInfoPtr->m_curTicks == milliseconds_ )
    {
        // Add this thread to the waiting list
        waiters_.put( *simInfoPtr );

        // Set flag/return code that I need to wait for the next simulated tick
        return true;
    }

    // Update my threads internal tick count 
    simInfoPtr->m_curTicks = milliseconds_;
    return false;
}


/////////////////////////////////////////
SimTick::SimTick()
    :m_curTicks( 0 ),
    m_ackPending( false ),
    m_threadId( 0 )
{
}


bool SimTick::isWaitingOnNextTick( size_t threadID ) noexcept
{
    bool waiting = false;

    myLock_.lock();
    SimTick* simInfoPtr = waiters_.head();
    while ( simInfoPtr )
    {
        if ( simInfoPtr->m_threadId == threadID )
        {
            waiting = true;
            break;
        }

        simInfoPtr = waiters_.next( *simInfoPtr );
    }
    myLock_.unlock();

    return waiting;
}


bool SimTick::usingSimTicks() noexcept
{
    // If my thread's SimInfo is NULL -->then I am NOT using simulated time
    return simTlsPtr_->get() != 0;
}


size_t SimTick::current( void ) noexcept
{
    Cpl::System::Mutex::ScopeBlock lock( myLock_ );
    return milliseconds_;
}


void SimTick::threadInit_( bool useSimTicks ) noexcept
{
    // Create the TLS key/index needed to store the per thread info (do a lazy create)
    if ( !simTlsPtr_ )
    {
        simTlsPtr_ = new( std::nothrow ) Tls();
    }

    // Set my TLS SimInfo block to NULL since the thread has be requested to NOT USE simulated ticks
    if ( !useSimTicks )
    {
        simTlsPtr_->set( 0 );
        return;
    }

    // Create an instance of SimTick for my per thread simInfo and store the newly created instance in TLS
    SimTick* simInfoPtr = new( std::nothrow ) SimTick();
    if ( !simInfoPtr )
    {
        FatalError::logRaw( "SimTick::threadInit_().  Failed to alloc an instance SimTick for thread", Thread::myId() );
    }

    simInfoPtr->m_threadId = Thread::myId();
    myLock_.lock();
    simInfoPtr->m_curTicks = milliseconds_;
    myLock_.unlock();
    simTlsPtr_->set( simInfoPtr );
}


void SimTick::onThreadExit_( void ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();


    // NOTHING needed if a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return;
    }


    // Check if I was 'processing' a tick
    if ( simInfoPtr->m_ackPending )
    {
        // Acknowledge that the thread has completed it's processing for the current tick
        simInfoPtr->m_ackPending  = false;
        tickSource_.signal();
    }

    // Free the SimTick object
    delete simInfoPtr;
}



/////////////////////////////////////////////
void Api::sleep( unsigned long milliseconds ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();

    // ALWAYS use the 'real' sleep call when I am a non-simulated-tick thread 
    if ( !simInfoPtr )
    {
        sleepInRealTime( milliseconds );
    }

    // Thread is using simulated time -->wait for my simulate ticks
    else
    {
        // Loop until enough simulated ticks have elapsed
        while ( milliseconds )
        {
            // Get the current time
            myLock_.lock();
            unsigned long start = milliseconds_;
            myLock_.unlock();

            // Wait on the tick
            SimTick::topLevelWait();
            myLock_.lock();
            unsigned long current = milliseconds_;
            myLock_.unlock();

            // Subtract the elapsed time from my count down timer
            unsigned long delta = (unsigned long) ElapsedTime::deltaMilliseconds( start, current );
            milliseconds        = milliseconds > delta ? milliseconds - delta : 0;
        }
    }
}


///////////////////////////////////////////////////////////////
unsigned long ElapsedTime::milliseconds( void ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();

    // ALWAYS use the 'real' elapsed time when I am a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return ElapsedTime::millisecondsInRealTime();
    }


    // Thread is using simulated time -->return the simulate time
    myLock_.lock();
    unsigned long msec = milliseconds_;
    myLock_.unlock();
    return msec;
}

unsigned long ElapsedTime::seconds( void ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();

    // ALWAYS use the 'real' elapsed time when I am a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return ElapsedTime::secondsInRealTime();
    }

    // Thread is using simulated time -->return the simulate time
    myLock_.lock();
    unsigned long seconds = seconds_;
    myLock_.unlock();
    return seconds;
}


ElapsedTime::Precision_T ElapsedTime::precision( void ) noexcept
{
    // Get my thread's SimInfo
    SimTick* simInfoPtr = (SimTick*) simTlsPtr_->get();

    // ALWAYS use the 'real' elapsed time when I am a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return ElapsedTime::precisionInRealTime();
    }


    // Thread is using simulated time -->return the simulate time
    myLock_.lock();
    Precision_T now;
    now.m_seconds      = seconds_;
    now.m_thousandths  = (unsigned long) ( milliseconds_ % 1000 );
    myLock_.unlock();
    return now;
}


