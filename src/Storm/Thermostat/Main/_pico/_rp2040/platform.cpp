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

#include "Storm/Thermostat/Main/Main.h"
#include "Storm/Thermostat/Main/_pico/Main.h"
#include "Storm/Thermostat/Main/Private_.h"
#include "Storm/Thermostat/SimHouse/Cmd.h"
#include "Storm/Thermostat/SimHouse/House.h"
#include "Storm/Thermostat/Logger.h"
#include "Storm/Thermostat/Outputs.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Cpl/System/Thread.h"
#include "Cpl/TShell/Cmd/Threads.h"

#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/MirroredChunk.h"
#include "Cpl/Persistent/NullRegionMedia.h"
#include "Storm/Thermostat/Main/UserRecord.h"
#include "Storm/Thermostat/Main/InstallerRecord.h"
#include "Storm/Thermostat/Main/RunTimeRecord.h"

#include "hardware/watchdog.h"

static Cpl::TShell::Cmd::Threads            cmdThreads_( g_cmdlist );
static Storm::Thermostat::SimHouse::Cmd     houseCmd_( g_cmdlist );


//
// Persistent Storage
//
// Note: My Board/platform currently does not have any persistent storage, but
//       plan is to 'add some' soon.  So I am including a Record Server even
//       though nothing actually persists

#define USER_REGION_START_ADDRESS           0
#define USER_REGION_LENGTH                  (28+200)

#define INSTALLER_REGION_START_ADDRESS      0
#define INSTALLER_REGION_LENGTH             (56+200)

#define RUNTIME_REGION_START_ADDRESS        0
#define RUNTIME_REGION_LENGTH               (4+200)


static Cpl::Persistent::NullRegionMedia         fd1UserRec_( USER_REGION_START_ADDRESS, USER_REGION_LENGTH );
static Cpl::Persistent::NullRegionMedia         fd2UserRec_( USER_REGION_START_ADDRESS, USER_REGION_LENGTH );
static Cpl::Persistent::MirroredChunk           chunkUserRec_( fd1UserRec_, fd2UserRec_ );
static Storm::Thermostat::Main::UserRecord      userRec_( chunkUserRec_ );

static Cpl::Persistent::NullRegionMedia         fd1InstallerRec_( INSTALLER_REGION_START_ADDRESS, INSTALLER_REGION_LENGTH );
static Cpl::Persistent::NullRegionMedia         fd2InstallerRec_( INSTALLER_REGION_START_ADDRESS, INSTALLER_REGION_LENGTH );
static Cpl::Persistent::MirroredChunk           chunkInstallerRec_( fd1InstallerRec_, fd2InstallerRec_ );
static Storm::Thermostat::Main::InstallerRecord installerRec_( chunkInstallerRec_ );

static Cpl::Persistent::NullRegionMedia         fd1RuntimeRec_( RUNTIME_REGION_START_ADDRESS, RUNTIME_REGION_LENGTH );
static Cpl::Persistent::NullRegionMedia         fd2RuntimeRec_( RUNTIME_REGION_START_ADDRESS, RUNTIME_REGION_LENGTH );
static Cpl::Persistent::MirroredChunk           chunkRuntimeRec_( fd1RuntimeRec_, fd2RuntimeRec_ );
static Storm::Thermostat::Main::RunTimeRecord   runtimeRec_( chunkRuntimeRec_ );

static Cpl::Persistent::Record*                 records_[3 + 1] ={ &userRec_, &installerRec_, &runtimeRec_, 0 };
static Cpl::Persistent::RecordServer            recordServer_( records_ );

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
    watchdog_enable( 1, false );
    while ( 1 );
    return exitCode;
}

///////////////////////////////////////////////////
void Storm::Thermostat::Logger::recordSystemData( Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    // No logging support (yet...)
}


void Storm::Thermostat::Outputs::updateHVACOutputs()
{
    // Not support yet...
}

