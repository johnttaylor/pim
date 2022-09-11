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
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"

#include <stdio.h>
#include "Cpl/System/Thread.h"

///
using namespace Cpl::System;



//////////////////////////////////////////////////
bool Semaphore::timedWait( unsigned long milliseconds ) noexcept
{
    // If not a simulated-tick thread -->do a 'regular Semaphore timed wait
    if ( !SimTick::usingSimTicks() )
    {
        return timedWaitInRealTime( milliseconds );
    }

    // Wait, 1 simulated tick at time, for the semaphore to be signaled
    unsigned long start = SimTick::current();
    while ( milliseconds )
    {
        // Exit when the semaphore has been signaled
        if ( tryWait() )
        {
            return true;
        }

        // Wait (at least) 1 tick for something to happen
        Api::sleep( 1 );

        // Update my count down timer
        unsigned long current = SimTick::current();
        unsigned long delta   = (unsigned long) ElapsedTime::deltaMilliseconds( start, current );
        start                 = current;
        milliseconds          = milliseconds > delta ? milliseconds - delta : 0;
    }
    return false;
}


void Semaphore::wait( void ) noexcept
{
    // If not a simulated-tick thread -->do a 'regular Semaphore wait
    if ( !SimTick::usingSimTicks() )
    {
        waitInRealTime();
        return;
    }

    SimTick::applicationWait();
    waitInRealTime();
}
