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

///
using namespace Cpl::System;



//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount )
{
    m_sema = CreateSemaphore( 0, initialCount, 0x7FFF, 0 ); // Create in the "EMPTY" state
}

Semaphore::~Semaphore()
{
    CloseHandle( m_sema );
}

int Semaphore::signal( void ) noexcept
{
    return ReleaseSemaphore( m_sema, 1, 0 ) ? 0 : -1;
}

int Semaphore::su_signal( void ) noexcept
{
    return ReleaseSemaphore( m_sema, 1, 0 ) ? 0 : -1;
}

bool Semaphore::tryWait( void ) noexcept
{
    DWORD result = WaitForSingleObject( m_sema, 0L );
    return result == WAIT_OBJECT_0 ? true : false;
}

void Semaphore::waitInRealTime( void ) noexcept
{
    WaitForSingleObject( m_sema, INFINITE );
}

bool Semaphore::timedWaitInRealTime( unsigned long timeout ) noexcept
{
    DWORD result = WaitForSingleObject( m_sema, (DWORD) timeout );
    return result == WAIT_OBJECT_0 ? true : false;
}
