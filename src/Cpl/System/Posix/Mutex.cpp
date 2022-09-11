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

#include "Cpl/System/Mutex.h"


//////////////////////////////////////////////////////////////////////////////
Cpl::System::Mutex::Mutex()
{
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init( &mutex_attr );
    pthread_mutexattr_settype( &mutex_attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &m_mutex, &mutex_attr );
}

Cpl::System::Mutex::~Mutex()
{
    // Guarantee that the mutex is unlocked before it is "destroyed"
    pthread_mutex_unlock( &m_mutex );
    pthread_mutex_destroy( &m_mutex );
}

void Cpl::System::Mutex::lock( void )
{
    pthread_mutex_lock( &m_mutex );
}


void Cpl::System::Mutex::unlock( void )
{
    pthread_mutex_unlock( &m_mutex );
}
