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
#include "Storm/Thermostat/Main/Private_.h"
#include "Storm/Thermostat/SimHouse/Cmd.h"
#include "Storm/Thermostat/SimHouse/House.h"
#include "Storm/Thermostat/Logger.h"
#include "Storm/Thermostat/Outputs.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Cpl/TShell/Cmd/FreeRTOS/Threads.h"
#include "Cpl/System/Thread.h"
#include "task.h"
#include <Adafruit_NeoPixel.h>

#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/MirroredChunk.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Storm/Thermostat/Main/UserRecord.h"
#include "Storm/Thermostat/Main/InstallerRecord.h"
#include "Storm/Thermostat/Main/RunTimeRecord.h"


static Cpl::TShell::Cmd::FreeRTOS::Threads  cmdThreads_( g_cmdlist );
static Storm::Thermostat::SimHouse::Cmd     houseCmd_( g_cmdlist );

static Storm::Thermostat::SimHouse::House   houseSimulator_;

// Get a reference to the NeoPixel driver
extern Adafruit_NeoPixel g_pixels;

// Persistent Storage
#define USER_REC_FILE_NAME_REGION1          "user1.nvr"
#define USER_REC_FILE_NAME_REGION2          "user2.nvr"
#define USER_REGION_START_ADDRESS           0
#define USER_REGION_LENGTH                  (28+200)

#define INSTALLER_REC_FILE_NAME_REGION1     "install1.nvr"
#define INSTALLER_REC_FILE_NAME_REGION2     "install2.nvr"
#define INSTALLER_REGION_START_ADDRESS      0
#define INSTALLER_REGION_LENGTH             (56+200)

#define RUNTIME_REC_FILE_NAME_REGION1       "runtime1.nvr"
#define RUNTIME_REC_FILE_NAME_REGION2       "runtime2.nvr"
#define RUNTIME_REGION_START_ADDRESS        0
#define RUNTIME_REGION_LENGTH               (4+200)


static Cpl::Persistent::FileAdapter             fd1UserRec_( USER_REC_FILE_NAME_REGION1, USER_REGION_START_ADDRESS, USER_REGION_LENGTH );
static Cpl::Persistent::FileAdapter             fd2UserRec_( USER_REC_FILE_NAME_REGION2, USER_REGION_START_ADDRESS, USER_REGION_LENGTH );
static Cpl::Persistent::MirroredChunk           chunkUserRec_( fd1UserRec_, fd2UserRec_ );
static Storm::Thermostat::Main::UserRecord      userRec_( chunkUserRec_ );

static Cpl::Persistent::FileAdapter             fd1InstallerRec_( INSTALLER_REC_FILE_NAME_REGION1, INSTALLER_REGION_START_ADDRESS, INSTALLER_REGION_LENGTH );
static Cpl::Persistent::FileAdapter             fd2InstallerRec_( INSTALLER_REC_FILE_NAME_REGION2, INSTALLER_REGION_START_ADDRESS, INSTALLER_REGION_LENGTH );
static Cpl::Persistent::MirroredChunk           chunkInstallerRec_( fd1InstallerRec_, fd2InstallerRec_ );
static Storm::Thermostat::Main::InstallerRecord installerRec_( chunkInstallerRec_ );

static Cpl::Persistent::FileAdapter             fd1RuntimeRec_( RUNTIME_REC_FILE_NAME_REGION1, RUNTIME_REGION_START_ADDRESS, RUNTIME_REGION_LENGTH );
static Cpl::Persistent::FileAdapter             fd2RuntimeRec_( RUNTIME_REC_FILE_NAME_REGION2, RUNTIME_REGION_START_ADDRESS, RUNTIME_REGION_LENGTH );
static Cpl::Persistent::MirroredChunk           chunkRuntimeRec_( fd1RuntimeRec_, fd2RuntimeRec_ );
static Storm::Thermostat::Main::RunTimeRecord   runtimeRec_( chunkRuntimeRec_ );

static Cpl::Persistent::Record*      records_[3 + 1] ={ &userRec_, &installerRec_, &runtimeRec_, 0 };
static Cpl::Persistent::RecordServer recordServer_( records_ );



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
    vTaskEndScheduler();
    return exitCode;
}

void Storm::Thermostat::Logger::recordSystemData( Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    // No logging support (yet...)
}


////////////////////////////////////////////////////////////////////////
#define HAS_CHANGED()   (prevSeqNumRelays != seqNumRelays || prevSeqNumNoActiveCond != seqNumNoActiveCond || prevSeqNumUserCfgMode != seqNumUserCfgMode || prevSeqNumIdtAlarms != seqNumIdtAlarms || prevSeqNumAirFilterAlert != seqNumAirFilterAlert )
#define SET_PREVIOUS()  prevSeqNumRelays = seqNumRelays; prevSeqNumNoActiveCond = seqNumNoActiveCond; prevSeqNumUserCfgMode = seqNumUserCfgMode; prevSeqNumIdtAlarms = seqNumIdtAlarms;  prevSeqNumAirFilterAlert = seqNumAirFilterAlert

#define G_PIXEL                 0
#define BK_PIXEL                1
#define W1_PIXEL                2
#define O_PIXEL                 3
#define Y1_PIXEL                4
#define IDT_ALARM_PIXEL         5
#define USER_CFG_ALARM_PIXEL    6
#define NO_COND_ALARM_PIXEL     6
#define AIR_FILTER_ALERT_PIXEL  7
#define NEXT_PIXEL_ROW          8

#define PIXEL_ON             64
#define PIXEL_OFF            0

void Storm::Thermostat::Outputs::updateHVACOutputs()
{
    static uint16_t prevSeqNumRelays         = Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN;
    static uint16_t prevSeqNumNoActiveCond   = Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN;
    static uint16_t prevSeqNumUserCfgMode    = Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN;
    static uint16_t prevSeqNumIdtAlarms      = Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN;
    static uint16_t prevSeqNumAirFilterAlert = Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN;

    // Get Output data
    Storm::Type::HvacRelayOutputs_T relays;
    Storm::Dm::MpSimpleAlarm::Data  noActiveCond;
    Storm::Dm::MpSimpleAlarm::Data  userCfgMode;
    Storm::Dm::MpSimpleAlarm::Data  airFilterAlert;
    Storm::Dm::MpIdtAlarm::Data     idt;
    uint16_t                        seqNumRelays;
    uint16_t                        seqNumNoActiveCond;
    uint16_t                        seqNumUserCfgMode;
    uint16_t                        seqNumIdtAlarms;
    uint16_t                        seqNumAirFilterAlert;
    if ( mp_relayOutputs.read( relays, &seqNumRelays ) == false ||
         mp_noActiveConditioningAlarm.read( noActiveCond, &seqNumNoActiveCond ) == false ||
         mp_userCfgModeAlarm.read( userCfgMode, &seqNumUserCfgMode ) == false ||
         mp_airFilterAlert.read( airFilterAlert, &seqNumAirFilterAlert ) == false ||
         mp_idtAlarms.read( idt, &seqNumIdtAlarms ) == false )
    {
        if ( HAS_CHANGED() )
        {
            // Turn everything off
            g_pixels.begin();
            for ( int i=0; i < BUILD_OPT_NUM_PIXELS; i++ )
            {
                g_pixels.setPixelColor( i, g_pixels.Color( 0, 0, 0, 0 ) );
            }
            g_pixels.show();
        }

        SET_PREVIOUS();
        return;
    }

    // LED Mapping:
    // G: Pixel  #0,  Green
    // BK: Pixel #1,  Green >0%
    // BK: Pixel #9,  Green >25%
    // BK: Pixel #17, Green >50%
    // BK: Pixel #25, Green >75%
    // W1: Pixel #2,  White
    // W2: Pixel #10, White
    // W3: Pixel #18, White
    // O:  Pixel #3,  Cooling: Blue, Heating: Red
    // Y1: Pixel #4,  Yellow
    // Y2: Pixel #12, Yellow
    // IDT Alarm, Pixel #5,  Primary IDT Fault,   yellow=non-critical, red=critical
    // IDT Alarm, Pixel #13, Secondary IDT Fault, yellow=non-critical, red=critical
    // User Cfg Mode Alarm,          Pixel #6,    yellow=non-critical, red-critical
    // No Active Conditioning Alarm, Pixel #14,   yellow=non-critical, red-critical
    // Air Filter Alert,             Pixel #7,    white=active
    if ( HAS_CHANGED() )
    {
        g_pixels.begin();

        g_pixels.setPixelColor( G_PIXEL, g_pixels.Color( 0, relays.g ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );

        g_pixels.setPixelColor( BK_PIXEL, g_pixels.Color( 0, relays.bk > 0 ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        g_pixels.setPixelColor( BK_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( 0, relays.bk > 25 ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        g_pixels.setPixelColor( BK_PIXEL + NEXT_PIXEL_ROW + NEXT_PIXEL_ROW, g_pixels.Color( 0, relays.bk > 50 ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        g_pixels.setPixelColor( BK_PIXEL + NEXT_PIXEL_ROW + NEXT_PIXEL_ROW + NEXT_PIXEL_ROW, g_pixels.Color( 0, relays.bk > 75 ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );

        g_pixels.setPixelColor( W1_PIXEL, g_pixels.Color( 0, 0, 0, relays.w1 ? PIXEL_ON : PIXEL_OFF ) );
        g_pixels.setPixelColor( W1_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( 0, 0, 0, relays.w2 ? PIXEL_ON : PIXEL_OFF ) );
        g_pixels.setPixelColor( W1_PIXEL + NEXT_PIXEL_ROW + NEXT_PIXEL_ROW, g_pixels.Color( 0, 0, 0, relays.w3 ? PIXEL_ON : PIXEL_OFF ) );

        g_pixels.setPixelColor( O_PIXEL, g_pixels.Color( relays.o ? PIXEL_OFF : PIXEL_ON, 0, relays.o ? PIXEL_ON : PIXEL_OFF, 0 ) );

        g_pixels.setPixelColor( Y1_PIXEL, g_pixels.Color( relays.y1 ? PIXEL_ON : PIXEL_OFF, relays.y1 ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        g_pixels.setPixelColor( Y1_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( relays.y2 ? PIXEL_ON : PIXEL_OFF, relays.y2 ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );

        if ( idt.critical )
        {
            g_pixels.setPixelColor( IDT_ALARM_PIXEL, g_pixels.Color( idt.primaryAlarm ? PIXEL_ON : PIXEL_OFF, 0, 0, 0 ) );
            g_pixels.setPixelColor( IDT_ALARM_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( idt.secondaryAlarm ? PIXEL_ON : PIXEL_OFF, 0, 0, 0 ) );
        }
        else
        {
            g_pixels.setPixelColor( IDT_ALARM_PIXEL, g_pixels.Color( idt.primaryAlarm ? PIXEL_ON : PIXEL_OFF, idt.primaryAlarm ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
            g_pixels.setPixelColor( IDT_ALARM_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( idt.secondaryAlarm ? PIXEL_ON : PIXEL_OFF, idt.secondaryAlarm ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        }

        if ( userCfgMode.critical )
        {
            g_pixels.setPixelColor( USER_CFG_ALARM_PIXEL, g_pixels.Color( userCfgMode.active ? PIXEL_ON : PIXEL_OFF, 0, 0, 0 ) );
        }
        else
        {
            g_pixels.setPixelColor( USER_CFG_ALARM_PIXEL, g_pixels.Color( userCfgMode.active ? PIXEL_ON : PIXEL_OFF, userCfgMode.active ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        }

        if ( noActiveCond.critical )
        {
            g_pixels.setPixelColor( NO_COND_ALARM_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( noActiveCond.active ? PIXEL_ON : PIXEL_OFF, 0, 0, 0 ) );
        }
        else
        {
            g_pixels.setPixelColor( NO_COND_ALARM_PIXEL + NEXT_PIXEL_ROW, g_pixels.Color( noActiveCond.active ? PIXEL_ON : PIXEL_OFF, noActiveCond.active ? PIXEL_ON : PIXEL_OFF, 0, 0 ) );
        }

        g_pixels.setPixelColor( AIR_FILTER_ALERT_PIXEL, g_pixels.Color( 0, 0, 0, airFilterAlert.active ? PIXEL_ON : PIXEL_OFF ) );

        g_pixels.show();
    }

    SET_PREVIOUS();
}

