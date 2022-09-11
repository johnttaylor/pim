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

#include "Cpl/System/Api.h"
#include "Cpl/System/Private_.h"
#include <time.h>


/// 
using namespace Cpl::System;

///
///
static Mutex systemMutex_;
static Mutex tracingMutex_;
static Mutex tracingOutputMutex_;
static Mutex sysList_;



////////////////////////////////////////////////////////////////////////////////
void Api::initialize( void )
{
    // Init the Colony.Core sub-systems
    StartupHook_::notifyStartupClients();
}


void Api::enableScheduling( void )
{
    // Assumption is that I am in user space, ergo thread schedule is already
    // started/active -->nothing needed!
}

bool Api::isSchedulingEnabled( void )
{
    return true; // Always return true since scheduling is always enabled by definition of running in user space
}

void Api::sleepInRealTime( unsigned long milliseconds ) noexcept
{
    // Convert milliseconds to the nanosleep time spec
    static const long nsec2msec = 1000000;
    static const long msec2sec  = 1000;
    time_t            sec       = milliseconds / msec2sec;
    long              nsec      = (milliseconds % msec2sec)*nsec2msec;
    const timespec    delay     = { sec,nsec };

    // Sleep
    nanosleep( &delay, 0 );
}


void Api::suspendScheduling(void)
{
    // DO NOT Nothing.  No native POSIX API for this - and one could argue that suspending scheduling on Desktop OS is bad thing
}

void Api::resumeScheduling(void)
{
    // DO NOT Nothing.  See above
}


////////////////////////////////////////////////////////////////////////////////
Mutex& Locks_::system( void )
{
    return systemMutex_;
}


Mutex& Locks_::tracing( void )
{
    return tracingMutex_;
}

Mutex& Locks_::sysLists( void )
{
    return sysList_;
}

Mutex& Locks_::tracingOutput( void )
{
    return tracingOutputMutex_;
}