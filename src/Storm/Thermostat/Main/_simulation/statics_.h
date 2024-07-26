#ifndef Storm_Thermostat_Main_simulation_statics_h
#define Storm_Thermostat_Main_simulation_statics_h
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
/** @file

    This file defines/creates persistent records and record server for the
    Thermostat simulation.  It also creates the House simulation and some
    additional objects that are common across simulation variants

    This file should ONLY be included by a SINGLE .cpp file!
 */

#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Persistent/MirroredChunk.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Storm/Thermostat/Main/InstallerRecord.h"
#include "Storm/Thermostat/Main/Main.h"
#include "Storm/Thermostat/Main/RunTimeRecord.h"
#include "Storm/Thermostat/Main/UserRecord.h"
#include "Storm/Thermostat/Outputs.h"
#include "Storm/Thermostat/SimHouse/Cmd.h"
#include "Storm/Thermostat/SimHouse/House.h"
#include "Storm/Thermostat/_file_logger/Log.h"

#ifndef DOXYGEN_WILL_SKIP_THIS

static Storm::Thermostat::SimHouse::Cmd houseCmd_(g_cmdlist);
static Storm::Thermostat::Log logCmd_(g_cmdlist);

static Storm::Thermostat::SimHouse::House houseSimulator_;

#define USER_REC_FILE_NAME_REGION1 "user.1.nvram"
#define USER_REC_FILE_NAME_REGION2 "user.2.nvram"
#define USER_REGION_START_ADDRESS 0
#define USER_REGION_LENGTH (28 + 200)

#define INSTALLER_REC_FILE_NAME_REGION1 "installer.1.nvram"
#define INSTALLER_REC_FILE_NAME_REGION2 "installer.2.nvram"
#define INSTALLER_REGION_START_ADDRESS 0
#define INSTALLER_REGION_LENGTH (56 + 200)

#define RUNTIME_REC_FILE_NAME_REGION1 "runtime.1.nvram"
#define RUNTIME_REC_FILE_NAME_REGION2 "runtime.2.nvram"
#define RUNTIME_REGION_START_ADDRESS 0
#define RUNTIME_REGION_LENGTH (4 + 200)

static Cpl::Persistent::FileAdapter fd1UserRec_(USER_REC_FILE_NAME_REGION1,
                                                USER_REGION_START_ADDRESS,
                                                USER_REGION_LENGTH);
static Cpl::Persistent::FileAdapter fd2UserRec_(USER_REC_FILE_NAME_REGION2,
                                                USER_REGION_START_ADDRESS,
                                                USER_REGION_LENGTH);
static Cpl::Persistent::MirroredChunk chunkUserRec_(fd1UserRec_, fd2UserRec_);
static Storm::Thermostat::Main::UserRecord userRec_(chunkUserRec_);

static Cpl::Persistent::FileAdapter
    fd1InstallerRec_(INSTALLER_REC_FILE_NAME_REGION1,
                     INSTALLER_REGION_START_ADDRESS, INSTALLER_REGION_LENGTH);
static Cpl::Persistent::FileAdapter
    fd2InstallerRec_(INSTALLER_REC_FILE_NAME_REGION2,
                     INSTALLER_REGION_START_ADDRESS, INSTALLER_REGION_LENGTH);
static Cpl::Persistent::MirroredChunk chunkInstallerRec_(fd1InstallerRec_,
                                                         fd2InstallerRec_);
static Storm::Thermostat::Main::InstallerRecord
    installerRec_(chunkInstallerRec_);

static Cpl::Persistent::FileAdapter
    fd1RuntimeRec_(RUNTIME_REC_FILE_NAME_REGION1, RUNTIME_REGION_START_ADDRESS,
                   RUNTIME_REGION_LENGTH);
static Cpl::Persistent::FileAdapter
    fd2RuntimeRec_(RUNTIME_REC_FILE_NAME_REGION2, RUNTIME_REGION_START_ADDRESS,
                   RUNTIME_REGION_LENGTH);
static Cpl::Persistent::MirroredChunk chunkRuntimeRec_(fd1RuntimeRec_,
                                                       fd2RuntimeRec_);
static Storm::Thermostat::Main::RunTimeRecord runtimeRec_(chunkRuntimeRec_);

static Cpl::Persistent::Record *records_[3 + 1] = {&userRec_, &installerRec_,
                                                   &runtimeRec_, 0};
static Cpl::Persistent::RecordServer recordServer_(records_);

#endif // DOXYGEN_WILL_SKIP_THIS
#endif // end Header latch
