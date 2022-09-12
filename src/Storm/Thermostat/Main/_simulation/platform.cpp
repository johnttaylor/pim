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

#include "Storm/Thermostat/Main/Private_.h"
#include "statics_.h"
#include "Cpl/System/Thread.h"
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
void initializePlatform0()
{
    // Create thread to run the House simulation
    Cpl::System::Thread::create( houseSimulator_, "HouseSim", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );

    // Create thread for persistent storage
    Cpl::System::Thread::create( recordServer_, "NVRAM", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
}

void openPlatform0()
{
    recordServer_.open();
}

void closePlatform0()
{
    recordServer_.close();
}

int exitPlatform( int exitCode )
{
    exit( exitCode );
    return exitCode;
}

void Storm::Thermostat::Outputs::updateHVACOutputs()
{
    // No outputs!
}

