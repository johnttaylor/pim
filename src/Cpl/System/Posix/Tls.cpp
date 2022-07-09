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

#include "Cpl/System/Tls.h"
#include "Cpl/System/FatalError.h"


///
using namespace Cpl::System;


/////////////////////////////////////////////////////////
Tls::Tls( void )
{
    if ( pthread_key_create( &m_key, NULL ) != 0 )
    {
        Cpl::System::FatalError::logRaw( "Posix::Tls::Tls().  pthread_key_create() failed when creating index to store TLS variable." );
    }
}

Tls::~Tls()
{
    pthread_key_delete( m_key );
}


/////////////////////////////////////////////////////////
void* Tls::get( void )
{
    return pthread_getspecific( m_key );
}

void Tls::set( void* newValue )
{
    pthread_setspecific( m_key, newValue );
}
