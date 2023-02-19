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

#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/Private_.h"
#include "pico/sync.h"


static critical_section_t globalCritSec_;

namespace {

/// This class is to 'zero' the elapsed to the start of the application
class RegisterInitHandler_ : public Cpl::System::StartupHook_
{
public:
    ///
    RegisterInitHandler_() :StartupHook_( eSYSTEM ) {}


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        critical_section_init( &globalCritSec_ );
    }
};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;

//////////////////////////////////////////////////////////////////////////////
void Cpl::System::GlobalLock::begin( void )
{
    critical_section_enter_blocking( &globalCritSec_ );
}

void Cpl::System::GlobalLock::end( void )
{
    critical_section_exit( &globalCritSec_ );
}



