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

#include "Cpl/System/Tls.h"
#include "Cpl/System/FatalError.h"


///
using namespace Cpl::System;

thread_local void* cpl_sytem_cpp11_tls_values_[OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES] = { 0, };
static bool        cpl_sytem_cpp11_tls_inUse_[OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES]  = { 0, };

/////////////////////////////////////////////////////////
Tls::Tls()
    :m_key( -1 )
{
    int i;
    for ( i=0; i < OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES; i++ )
    {
        if ( cpl_sytem_cpp11_tls_inUse_[i] == false )
        {
            cpl_sytem_cpp11_tls_inUse_[i] = true;
            m_key                         = i;
            break;
        }
    }

    if ( m_key < 0 )
    {
        Cpl::System::FatalError::logRaw( "Cpp11::Tls::Tls().  Exceeded OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES limit." );
    }
}

Tls::~Tls()
{
    cpl_sytem_cpp11_tls_inUse_[m_key] = false;
}


/////////////////////////////////////////////////////////
void* Tls::get( void )
{
    return cpl_sytem_cpp11_tls_values_[m_key];
}


void Tls::set( void* newValue )
{
    cpl_sytem_cpp11_tls_values_[m_key] = newValue;
}
