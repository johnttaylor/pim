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
    : m_sema( initialCount )
{
    // Nothing needed.  The initialization of the semaphore is done when the Cpl C++ library is initialized
}

Semaphore::~Semaphore()
{
    // Nothing needed
}

int Semaphore::signal( void ) noexcept
{
    return sem_release( m_sema.m_rp2040Sema )? 0: 1;    // Return zero on success
}

int Semaphore::su_signal( void ) noexcept
{
    return sem_release( m_sema.m_rp2040Sema ) ? 0 : 1;  // Return zero on success
}

bool Semaphore::tryWait( void ) noexcept
{
    return sem_try_acquire( m_sema.m_rp2040Sema );
}

void Semaphore::waitInRealTime( void ) noexcept
{
    sem_acquire_blocking( m_sema.m_rp2040Sema );
}

bool Semaphore::timedWaitInRealTime( unsigned long timeout ) noexcept
{
    return sem_acquire_timeout_ms( m_sema.m_rp2040Sema, timeout );
}

//////////////////////////////////////////////////
// Simulated time NOT supported
bool Semaphore::timedWait( unsigned long timeout ) noexcept
{
    return sem_acquire_timeout_ms( m_sema.m_rp2040Sema, timeout );
}

void Semaphore::wait( void ) noexcept
{
    sem_acquire_blocking( m_sema.m_rp2040Sema );
}
