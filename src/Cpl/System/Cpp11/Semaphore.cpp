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

#include "Cpl/System/Semaphore.h"
#include <chrono>


///
using namespace Cpl::System;

#define MAX_COUNT_  0x7FFF


//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount )
{
    m_sema.m_count = initialCount;
}

Semaphore::~Semaphore()
{
}


int Semaphore::signal( void ) noexcept
{
    std::unique_lock<std::mutex> lock( m_sema.m_mutex );
    if ( ++(m_sema.m_count) > MAX_COUNT_ )
    {
        m_sema.m_count = MAX_COUNT_;
    }
    m_sema.m_cond.notify_one();
    return 0; // Alway return success!
}


int Semaphore::su_signal( void ) noexcept
{
    return signal();
}


bool Semaphore::tryWait( void ) noexcept
{
    bool decremented = false;
    std::unique_lock<std::mutex> lock( m_sema.m_mutex );
    if ( m_sema.m_count > 0 )
    {
        m_sema.m_count--;
        decremented = true;
    }

    return decremented;
}


void Semaphore::waitInRealTime( void ) noexcept
{
    std::unique_lock<std::mutex> lock( m_sema.m_mutex );
    while ( m_sema.m_count == 0 )
    {
        m_sema.m_cond.wait( lock );
    }

    m_sema.m_count--;
}


bool Semaphore::timedWaitInRealTime( unsigned long milliseconds ) noexcept
{
    auto start = std::chrono::system_clock::now();
    auto end   = start + std::chrono::milliseconds( milliseconds );

    // Wait until: signaled, or timeout (but beware of spurious wake-ups from the Condition variable)
    std::unique_lock<std::mutex> lock( m_sema.m_mutex );
    while ( m_sema.m_count == 0 && std::chrono::system_clock::now() < end )
    {
        m_sema.m_cond.wait_until( lock, end );
    }

    bool signaled = false;
    if ( m_sema.m_count != 0 )
    {
        m_sema.m_count--;
        signaled = true;
    }

    return signaled;
}
