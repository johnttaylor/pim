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

#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/BareMetal/Hal_.h"
#include <time.h>
#include <windows.h>
/// 


///////////////////////////////////////////////////////////////
void Cpl::System::BareMetal::initialize( void ) noexcept
{
    // Nothing needed
}

void Cpl::System::BareMetal::busyWait( unsigned long waitTimeMs ) noexcept
{
    Sleep( (DWORD) waitTimeMs );
}

unsigned long Cpl::System::BareMetal::getElapsedTime( void ) noexcept
{
    return clock();
}


///////////////////////////////////////////////////////////////
void Cpl::System::GlobalLock::begin( void )
{
    // Do nothing
}

void Cpl::System::GlobalLock::end( void )
{
    // Do nothing
}