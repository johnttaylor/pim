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

///
static void* values_[OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES];
static int   next_index_ = 0;


/////////////////////////////////////////////////////////
Tls::Tls( void )
{
    m_key = next_index_++;
    if ( next_index_ >= OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES )
    {
        Cpl::System::FatalError::logRaw( "BareMetal::Tls::Tls().  Out of allocated indexes to store TLS variable." );
    }
}

Tls::~Tls()
{
    // Cheat here and do nothing, i.e. will not be a to 'free' a previously allocated TLS
}


/////////////////////////////////////////////////////////
void* Tls::get( void )
{
    return values_[m_key];
}

void Tls::set( void* newValue )
{
    values_[m_key] = newValue;
}
