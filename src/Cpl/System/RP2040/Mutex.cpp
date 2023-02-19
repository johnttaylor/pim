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
    // Nothing needed.  The initialization of the mutex is done when the Cpl C++ library is initialized
}

Cpl::System::Mutex::~Mutex()
{
    // Nothing needed
}

void Cpl::System::Mutex::lock( void )
{
    if ( m_mutex.m_rp2040Mutex )
    {
        recursive_mutex_enter_blocking( m_mutex.m_rp2040Mutex );
    }
}

void Cpl::System::Mutex::unlock( void )
{
    if ( m_mutex.m_rp2040Mutex )
    {
        recursive_mutex_exit( m_mutex.m_rp2040Mutex );
    }
}


