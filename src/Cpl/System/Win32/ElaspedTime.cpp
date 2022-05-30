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

#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Private_.h"
#include <stdlib.h>


/// 
using namespace Cpl::System;



///////////////////////////////////////////////////////////////
static unsigned long elapsedMsec_;
static unsigned long lastMsec_;

namespace {

/// This class is to 'zero' the elapsed to the start of the application
class RegisterInitHandler_ : public Cpl::System::StartupHook_
{
public:
    ///
    RegisterInitHandler_():StartupHook_( eSYSTEM ) {}


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        elapsedMsec_  = 0;
        lastMsec_     = clock();
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


///////////////////////////////////////////////////////////////
unsigned long ElapsedTime::millisecondsInRealTime( void ) noexcept
{
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::system() );

    unsigned long newTime = clock();
    unsigned long delta   = newTime - lastMsec_;
    elapsedMsec_         += delta;
    lastMsec_             = newTime;

    return elapsedMsec_;
}

unsigned long ElapsedTime::secondsInRealTime( void ) noexcept
{
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::system() );

    // Update my internal elapsedMsec time
    millisecondsInRealTime();

    // Convert my internal elapsed time to seconds
    return (unsigned long) (elapsedMsec_ / 1000LL);
}


ElapsedTime::Precision_T ElapsedTime::precisionInRealTime( void ) noexcept
{
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::system() );

    // Update my internal elapsedMsec time
    millisecondsInRealTime();

    // Convert to my Precision format
    Precision_T now;
    ldiv_t      result = ldiv( elapsedMsec_, 1000L );
    now.m_seconds      = (unsigned long) result.quot;
    now.m_thousandths  = (uint16_t) result.rem;
    return now;
}
