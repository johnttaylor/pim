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

#include "Runnable.h"
#include "Private_.h"


///
using namespace Cpl::System;


/////////////////////////////////////////////////////////////
Runnable::Runnable( void )
    :m_running( false )
{
}

void Runnable::run( void )
{
    // Set my running state
    Locks_::system().lock();
    m_running = true;
    Locks_::system().unlock();

    // Run my application code
    appRun();

    // Clear my running state
    Locks_::system().lock();
    m_running = false;
    Locks_::system().unlock();
}

bool Runnable::isRunning( void ) noexcept
{
    bool result = false;

    Locks_::system().lock();
    result = m_running;
    Locks_::system().unlock();

    return result;
}
