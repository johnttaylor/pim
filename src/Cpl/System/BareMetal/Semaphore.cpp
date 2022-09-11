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

#include "Cpl/System/Semaphore.h"
#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/ElapsedTime.h"

///
using namespace Cpl::System;



//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount )
{
    m_sema = initialCount;
}

Semaphore::~Semaphore()
{
    // Nothing needed
}

int Semaphore::signal( void ) noexcept
{
    GlobalLock::begin();
    su_signal();
    GlobalLock::end();
    return 0;
}

int Semaphore::su_signal( void ) noexcept
{
    // Increment the semaphore (but don't let it roll over)
    m_sema++;
    if ( m_sema == 0 )
    {
        m_sema--;
    }
    return 0;
}

bool Semaphore::tryWait( void ) noexcept
{
    bool signaled = false;
    GlobalLock::begin();
    if ( m_sema > 0 )
    {
        m_sema--;
        signaled = true;
    }
    GlobalLock::end();

    return signaled;
}

void Semaphore::waitInRealTime( void ) noexcept
{
    for(;;)
    {
        GlobalLock::begin();
        if ( m_sema > 0 )
        {
            m_sema--;
            GlobalLock::end();
            return;
        }
        GlobalLock::end();
    }
}

bool Semaphore::timedWaitInRealTime( unsigned long timeout ) noexcept
{
    unsigned long start = ElapsedTime::milliseconds();

    for ( ;;)
    {
        GlobalLock::begin();
        if ( m_sema > 0 )
        {
            m_sema--;
            GlobalLock::end();
            return true;
        }
        GlobalLock::end();

        if ( ElapsedTime::expiredMilliseconds( start, timeout ) )
        {
            return false;
        }
    }
}

bool Semaphore::timedWait( unsigned long timeout ) noexcept
{
    return timedWaitInRealTime( timeout );
}

void Semaphore::wait( void ) noexcept
{
    waitInRealTime();
}