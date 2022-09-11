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
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Private_.h"
#include "Thread.h"

///
using namespace Cpl::System;


static uint16_t nextTlsIndex_;  // This guarantied to be initialized to zero (per the C++ standard) because it is in the BSS segment 


/////////////////////////////////////////////////////////
Tls::Tls( void )
    :m_key( (Cpl_System_TlsKey_T) -1 )
{
    // Check if there are any TLS indexes/variables left
    Cpl::System::Locks_::system().lock();
    if ( nextTlsIndex_ < OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES )
    {
        m_key = nextTlsIndex_++;
    }
    Cpl::System::Locks_::system().unlock();


    // Generate a fatal error if I am out of TLS indexes/variables
    if ( m_key == (Cpl_System_TlsKey_T) -1 )
    {
        Cpl::System::FatalError::logRaw( "FreeRTOS::Tls::Tls().  Exceeded TLS indexes.", nextTlsIndex_ );
    }
}


Tls::~Tls()
{
    /* Nothing needed because the current implementation DOES NOT recycle
       TLS indexes.  This is because there is no good/easy/clean way to go
       in any reset the TLS value to zero (across ALL active threads) - which
       is the guaranteed initial value of all TLS indexes/keys when created.
    */
}


/////////////////////////////////////////////////////////
void* Tls::get( void )
{
    // Get access to my FreeRTOS thread object.
    void** myArray = Cpl::System::FreeRTOS::Thread::getTlsArray();
    return myArray[m_key];
}

void Tls::set( void* newValue )
{
    // Get access to my FreeRTOS thread object.
    void** myArray = Cpl::System::FreeRTOS::Thread::getTlsArray();
    myArray[m_key] = newValue;
}
