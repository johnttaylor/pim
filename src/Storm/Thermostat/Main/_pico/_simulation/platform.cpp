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

#include "Storm/Thermostat/Main/_pico/Main.h"
#include "Storm/Thermostat/Main/Private_.h"
#include "Storm/Thermostat/Main/_simulation/statics_.h"
#include "Cpl/System/Thread.h"
#include <stdlib.h>

// The Algorithm shares the same thread as the Record server.  In theory I
// can get away with this because the Algorithm executes at 0.5 Hz - which
// should be slow enough that any blocking/busy writes the record server
// performs will NOT negatively impact the Algorithm timing.
Cpl::Dm::MailboxServer*                         g_algoRunnablePtr = &recordServer_;


////////////////////////////////////////////////////////////////////////////////
void initializePlatform0()
{
    // Note: The 'main' code creates the thread for the RecordServer/Algorithm Runnable object
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

