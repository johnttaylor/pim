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

#include "colony_config.h"

#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/Logging/EntryData_T.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Persistent/MirroredChunk.h"
#include "Cpl/Persistent/CrcChunk.h"
#include "Cpl/Persistent/IndexedEntryRecord.h"
#include "Cpl/Persistent/IndexRecord.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/IndexedEntryServer.h"
#include "Cpl/Container/RingBufferMP.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Uint64.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/TShell/Maker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/TShell/Cmd/Wait.h"
#include "Cpl/Logging/TShell/Log.h"
#include "Cpl/Logging/Api.h"
#include "Cpl/Type/enum.h"
#include "Cpl/Logging/TimeApi.h"
#include "Cpl/System/ElapsedTime.h"
#include <stdio.h>


#define FILE_NAME_INDEX_REGION1   "index1.bin"
#define FILE_NAME_INDEX_REGION2   "index2.bin"
#define FILE_NAME_ENTRY_REGION    "entries.bin"

#define MAX_ENTRIES                 7   
#define ENTRY_REGION_SIZE           (TOTAL_ENTRY*MAX_ENTRIES)

#define ENTRY_MAX_LEN               Cpl::Logging::EntryData_T::entryLen
#define CHUNK_META_LEN              (4+sizeof(size_t))
#define ENTRY_META                  8
#define TOTAL_ENTRY                 (ENTRY_META+CHUNK_META_LEN+ENTRY_MAX_LEN)

#define MIRROR_CHUNK_META_LEN       (sizeof(size_t) + sizeof(uint32_t) + sizeof(uint64_t))
#define INDEX_RECORD_LEN            (sizeof(size_t) + 8)
#define TOTAL_INDEX_RECORD			(INDEX_RECORD_LEN+MIRROR_CHUNK_META_LEN)
/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

BETTER_ENUM( CategoryId, uint32_t
             , CRITICAL    = 0x00000001
             , WARNING     = 0x00000002
             , EVENT       = 0x00000004 );

BETTER_ENUM( CriticalMsg, uint16_t, OUT_OF_MEMORY, BOB_HAPPENED );
BETTER_ENUM( WarningMsg, uint16_t, BATTERY_LOW, CONNECTION_DROPPED, LOGGING_OVERFLOW );
BETTER_ENUM( EventMsg, uint16_t, ELVIS_HAS_LEFT_THE_BUILDING, SKY_IS_FALLING );

bool Cpl::Logging::getIDStrings( uint32_t             categoryNumericValue,
                                 uint16_t             messageIdNumericValue,
                                 Cpl::Text::String&   dstCategoryString,
                                 Cpl::Text::String&   dstMessageString ) noexcept
{
    switch ( categoryNumericValue )
    {
    case CategoryId::CRITICAL:
        if ( CriticalMsg::_is_valid( messageIdNumericValue ) )
        {
            dstCategoryString = (+CategoryId::CRITICAL)._to_string();
            dstMessageString  = CriticalMsg::_from_integral_unchecked( messageIdNumericValue )._to_string();
            return true;
        }
        break;
    case CategoryId::WARNING:
        if ( WarningMsg::_is_valid( messageIdNumericValue ) )
        {
            dstCategoryString = (+CategoryId::WARNING)._to_string();
            dstMessageString  = WarningMsg::_from_integral_unchecked( messageIdNumericValue )._to_string();
            return true;
        }
        break;
    case CategoryId::EVENT:
        if ( EventMsg::_is_valid( messageIdNumericValue ) )
        {
            dstCategoryString = (+CategoryId::EVENT)._to_string();
            dstMessageString  = EventMsg::_from_integral_unchecked( messageIdNumericValue )._to_string();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}


static CplLoggingTime_T nowValue_;
CplLoggingTime_T Cpl::Logging::now() noexcept
{
    return nowValue_++;
}

////////////////////////////////////////////////////////////////////////////////
// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::Mp::Uint64    mp_index_( modelDb_, "index" );
static Cpl::Dm::Mp::Uint32    mp_elemCount_( modelDb_, "count" );


static Cpl::Persistent::FileAdapter     indexFd1_( FILE_NAME_INDEX_REGION1, 0, TOTAL_INDEX_RECORD );
static Cpl::Persistent::FileAdapter     indexFd2_( FILE_NAME_INDEX_REGION2, 0, TOTAL_INDEX_RECORD );
static Cpl::Persistent::FileAdapter     entriesFd_( FILE_NAME_ENTRY_REGION, 0, ENTRY_REGION_SIZE );
static Cpl::Persistent::MirroredChunk   indexRecChunk_( indexFd1_, indexFd2_ );
static Cpl::Persistent::CrcChunk        entriesChunk_( entriesFd_ );

#define MAX_BUFFER_SIZE 10
static Cpl::Logging::EntryData_T memoryEntryBuffer_[MAX_BUFFER_SIZE];
static Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T> entryBuffer_( MAX_BUFFER_SIZE, memoryEntryBuffer_, mp_elemCount_ );

static Cpl::Persistent::IndexRecord										indexRecord_( indexRecChunk_ );
static Cpl::Persistent::IndexedEntryRecord								logEntryRecord_( entriesChunk_, ENTRY_MAX_LEN, entriesFd_, indexRecord_, mp_index_ );
static Cpl::Persistent::Record*											records_[2] ={ &logEntryRecord_, 0 };
static Cpl::Persistent::RecordServer									recordServer_( records_ );
static Cpl::Persistent::IndexedEntryServer<Cpl::Logging::EntryData_T>	logServer_( recordServer_, logEntryRecord_, entryBuffer_ );

static Cpl::System::Thread* t1_ = nullptr;

///////////////////////////////////////
namespace {
class ShutdownHack : public Cpl::TShell::Cmd::Command
{
public:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return "shutdown"; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return "  Hack to stop the Persistent Storage Server"; }



public:
    /// Constructor
    ShutdownHack( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Cpl::TShell::Security::Permission_T minPerm = Cpl::TShell::Security::ePUBLIC ) noexcept
        :Command( commandList, "shutdown", minPerm )
    {
    }


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
    {
        logServer_.close();
        recordServer_.close();
        recordServer_.pleaseStop();
        Cpl::System::Api::sleep( 500 );
        Cpl::System::Thread::destroy( *t1_ );
        context.writeFrame( "The Persistent Storage thread has been shutdown" );
        return Command::eSUCCESS;

    }
};
} // end anonymous namespace
///////////////////////////////////////

static Cpl::Container::Map<Cpl::TShell::Command>	cmdlist_;
static Cpl::TShell::Maker							cmdProcessor_( cmdlist_ );
static Cpl::TShell::Stdio							shell_( cmdProcessor_ );
static Cpl::TShell::Cmd::Help						helpCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Bye						byeCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Trace						traceCmd_( cmdlist_ );
static Cpl::TShell::Cmd::TPrint						tprintCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Wait						waitCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Threads				    threadsCmd_( cmdlist_ );

static Cpl::Logging::TShell::Log					logCmd_( cmdlist_, recordServer_, logServer_ );
static ShutdownHack                                 shutdownCmd_( cmdlist_ );


void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    t1_ = Cpl::System::Thread::create( recordServer_, "PERSIST" );
    CPL_SYSTEM_ASSERT( t1_ );

    // Start the shell
    shell_.launch( infd, outfd );

    // Start the scheduler
    Cpl::System::Api::enableScheduling();

    Cpl::Logging::initialize( entryBuffer_,
                              CategoryId::WARNING,
                              (+CategoryId::WARNING)._to_string(),
                              WarningMsg::LOGGING_OVERFLOW,
                              (+WarningMsg::LOGGING_OVERFLOW)._to_string() );
    recordServer_.open();
    logServer_.open();

    // Wait forever - the 'bye' command is responsible for exiting
    Cpl::System::Api::sleep( 0xFFFFFFFF );
}

