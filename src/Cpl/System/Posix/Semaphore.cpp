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
#include <errno.h>

///
using namespace Cpl::System;



//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount )
{
    sem_init( &m_sema, 0, initialCount );
}

Semaphore::~Semaphore()
{
    sem_destroy( &m_sema );
}

int Semaphore::signal( void ) noexcept
{
    return sem_post( &m_sema );
}

int Semaphore::su_signal( void ) noexcept
{
    // TODO: Add log unexpected error/event since this isn't really supported
    return sem_post( &m_sema );
}

bool Semaphore::tryWait( void ) noexcept
{
    int result = sem_trywait( &m_sema );
    return result == -1 && errno == EAGAIN ? false : true;
}

void Semaphore::waitInRealTime( void ) noexcept
{
    sem_wait( &m_sema );
}


bool Semaphore::timedWaitInRealTime( unsigned long timeout ) noexcept
{
    // Convert milliseconds to the nanosleep time spec
    static const long nsec2msec = 1000000;
    static const long msec2sec  = 1000;
    time_t            sec       = timeout / msec2sec;
    long              nsec      = (timeout % msec2sec)*nsec2msec;

    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    ts.tv_sec  += sec;
    ts.tv_nsec += nsec;
    if ( ts.tv_nsec > 999999999 )
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    int result = sem_timedwait( &m_sema, &ts );
    return result == -1 && errno == ETIMEDOUT ? false : true;
}

