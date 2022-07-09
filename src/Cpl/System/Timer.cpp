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
/** @file */

#include "Timer.h"
#include "Assert.h"


///
using namespace Cpl::System;

/////////////////////////////
Timer::Timer( TimerManager& timingSource )
    :m_timingSource( &timingSource )
    , m_count( 0 )
{
}

Timer::Timer()
    : m_timingSource( 0 )
    , m_count( 0 )
{
}
void Timer::setTimingSource( TimerManager& timingSource ) noexcept
{
    m_timingSource = &timingSource;
}

/////////////////////////////
void Timer::start( unsigned long timerDurationInMilliseconds ) noexcept
{
    CPL_SYSTEM_ASSERT( m_timingSource );
    m_timingSource->detach( *this );
    m_count = m_timingSource->msecToCounts( timerDurationInMilliseconds );
    m_timingSource->attach( *this );
}

void Timer::stop() noexcept
{
    CPL_SYSTEM_ASSERT( m_timingSource );
    m_timingSource->detach( *this );
}


void Timer::decrement( unsigned long milliseconds ) noexcept
{
    m_count -= milliseconds;
}
void Timer::increment( unsigned long milliseconds ) noexcept
{
    m_count += milliseconds;
}

unsigned long Timer::count() const noexcept
{
    return m_count;
}
