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

#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/BareMetal/Hal_.h"
#include <time.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////
void Cpl::System::BareMetal::initialize( void ) noexcept
{
    // Nothing needed
}

void Cpl::System::BareMetal::busyWait( unsigned long waitTimeMs ) noexcept
{
    // Convert milliseconds to the nanosleep time spec
    static const long nsec2msec = 1000000;
    static const long msec2sec  = 1000;
    time_t            sec       = waitTimeMs / msec2sec;
    long              nsec      = (waitTimeMs % msec2sec)*nsec2msec;
    const timespec    delay     = { sec,nsec };

    // Sleep
    nanosleep( &delay, 0 );
}

unsigned long Cpl::System::BareMetal::getElapsedTime( void ) noexcept
{
    struct timespec tm;
    clock_gettime( CLOCK_MONOTONIC, &tm );
    return tm.tv_sec * 1000 + tm.tv_nsec / 1000000;
}


///////////////////////////////////////////////////////////////
void Cpl::System::GlobalLock::begin( void )
{
    // Do nothing
}

void Cpl::System::GlobalLock::end( void )
{
    // Do nothing
}