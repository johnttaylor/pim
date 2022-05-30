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

#include "Catch/catch.hpp"
#include "Catch/helpers.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Persistent/IndexedEntryServer.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/CrcChunk.h"
#include "Cpl/Persistent/MirroredChunk.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/Mp/Uint64.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Itc/SyncReturnHandler.h"

#define SECT_   "_0test"

#define FILE_NAME_INDEX_REGION1   "index1.nvram"
#define FILE_NAME_INDEX_REGION2   "index2.nvram"
#define FILE_NAME_ENTRY_REGION    "entries.nvram"

#define MAX_ENTRIES                 7   
#define ENTRY_REGION_SIZE           ((ENTRY_META + ENTRY_META + CHUNK_META_LEN)*MAX_ENTRIES)

#define ENTRY_MAX_LEN               8
#define CHUNK_META_LEN              (4+sizeof(size_t))
#define ENTRY_META                  8
#define TOTAL_ENTRY                 (ENTRY_META+CHUNK_META_LEN+ENTRY_META)

#define ENTRY1                  "Hello"
#define ENTRY2                  "World"
#define ENTRY3                  "GoodBy"

using namespace Cpl::Persistent;

namespace {

class MyIndexedEntryRecord : public IndexedEntryRecord
{
public:
    MyIndexedEntryRecord( Chunk&                  entryChunkHandler,
                          size_t                  singleEntrySizeInBytes,
                          RegionMedia&            entryRegion,
                          IndexRecord&            secondaryRecord,
                          Cpl::Dm::Mp::Uint64&    mpForLatestIndexValue ) noexcept
        :IndexedEntryRecord( entryChunkHandler, singleEntrySizeInBytes, entryRegion, secondaryRecord, mpForLatestIndexValue )
    {
    }
public:
    size_t getTotalEntryLength()
    {
        return ENTRY_MAX_LEN + getMetadataLength() + m_chunkHandler.getMetadataLength();
    }
};
class AppEntryPayload : public Payload
{
public:
    AppEntryPayload() { memset( m_buffer, 0, ENTRY_MAX_LEN ); }

public:
    void appSet( const char* newValue )
    {
        m_buffer[0] = '\0';
        strncat( m_buffer, newValue, ENTRY_MAX_LEN - 1 );
    }

public:
    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        REQUIRE( maxDstLen >= ENTRY_MAX_LEN );
        memcpy( dst, m_buffer, ENTRY_MAX_LEN );
        return ENTRY_MAX_LEN;
    }

    bool putData( const void* src, size_t srcLen ) noexcept
    {
        REQUIRE( ENTRY_MAX_LEN <= srcLen );
        memcpy( m_buffer, src, ENTRY_MAX_LEN );
        return true;
    }

public:
    char m_buffer[ENTRY_MAX_LEN];
};

}; // end anonymous namespace

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::Mp::Uint64    mp_index_( modelDb_, "index"  );
static Cpl::Dm::Mp::Uint32    mp_elemCount_( modelDb_, "count" );


static Cpl::Persistent::FileAdapter     indexFd1( FILE_NAME_INDEX_REGION1, 0, 128 );
static Cpl::Persistent::FileAdapter     indexFd2( FILE_NAME_INDEX_REGION2, 0, 128 );
static Cpl::Persistent::FileAdapter     entriesFd( FILE_NAME_ENTRY_REGION, 0, ENTRY_REGION_SIZE );
static Cpl::Persistent::MirroredChunk   indexRecChunk( indexFd1, indexFd2 );
static Cpl::Persistent::CrcChunk        entriesChunk( entriesFd );

#define MAX_BUFFER_SIZE 10
static AppEntryPayload memoryEntryBuffer_[MAX_BUFFER_SIZE];
static Cpl::Container::RingBufferMP<AppEntryPayload> entryBuffer_( MAX_BUFFER_SIZE, memoryEntryBuffer_, mp_elemCount_ );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "IndexedEntryServer" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "INDEXED-ENTRY-RECORD Test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    mp_index_.setInvalid();
    AppEntryPayload      appPayload;
    IndexRecord          indexRecord( indexRecChunk );
    MyIndexedEntryRecord entryRecord( entriesChunk, ENTRY_MAX_LEN, entriesFd, indexRecord, mp_index_ );
    Cpl::Persistent::Record* records[2] = { &entryRecord, 0 };
    Cpl::Persistent::RecordServer recordServer( records );
    IndexedEntryServer<AppEntryPayload> uut( recordServer, entryRecord, entryBuffer_ );
    Cpl::System::Thread* t1 = Cpl::System::Thread::create( recordServer, "UUT" );
    REQUIRE( t1 );

    size_t actualEntrySize = entryRecord.getTotalEntryLength();

    SECTION( "no persistent data" )
    {
        // Delete files
        Cpl::Io::File::Api::remove( FILE_NAME_INDEX_REGION1 );
        Cpl::Io::File::Api::remove( FILE_NAME_INDEX_REGION2 );
        Cpl::Io::File::Api::remove( FILE_NAME_ENTRY_REGION );

        // No persistent data
        recordServer.open();
        uut.open();

        // Add entries
        appPayload.appSet( ENTRY1 );
        REQUIRE( entryBuffer_.add( appPayload ) );
        appPayload.appSet( ENTRY2 );
        REQUIRE( entryBuffer_.add( appPayload ) );
        appPayload.appSet( ENTRY3 );
        REQUIRE( entryBuffer_.add( appPayload ) );

        // Wait for changes...
        REQUIRE( minWaitOnModelPoint<Cpl::Dm::Mp::Uint32, uint32_t>( mp_elemCount_, 0, 1000 ) );
        REQUIRE( minWaitOnModelPoint<Cpl::Dm::Mp::Uint64, uint64_t>( mp_index_, 3, 10 ) );

        appPayload.appSet( "" );
        GetLatestRequest::Payload      payload( appPayload );
        Cpl::Itc::SyncReturnHandler    srh;
        GetLatestRequest::GetLatestMsg msg( uut, payload, srh );
        recordServer.postSync( msg );
        REQUIRE( payload.m_success == true );
        REQUIRE( payload.m_markerEntryRetreived.mediaOffset == actualEntrySize * 3 );
        REQUIRE( payload.m_markerEntryRetreived.indexValue == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        uut.close();
        recordServer.close();
    }
    SECTION( "startup - with data" )
    {

        recordServer.open();
        uut.open();

        appPayload.appSet( "" );
        GetLatestRequest::Payload      payload( appPayload );
        Cpl::Itc::SyncReturnHandler    srh;
        GetLatestRequest::GetLatestMsg msg( uut, payload, srh );
        recordServer.postSync( msg );
        REQUIRE( payload.m_success == true );
        REQUIRE( payload.m_markerEntryRetreived.mediaOffset == actualEntrySize * 3  );
        REQUIRE( payload.m_markerEntryRetreived.indexValue == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        appPayload.appSet( "" );
        GetNextRequest::Payload      payload2( appPayload, payload.m_markerEntryRetreived, payload.m_markerEntryRetreived.indexValue );
        Cpl::Itc::SyncReturnHandler  srh2;
        GetNextRequest::GetNextMsg   msg2( uut, payload2, srh2 );
        recordServer.postSync( msg2 );
        REQUIRE( payload2.m_success == false );

        appPayload.appSet( "" );
        GetPreviousRequest::Payload         payload3( appPayload, payload.m_markerEntryRetreived, payload.m_markerEntryRetreived.indexValue );
        Cpl::Itc::SyncReturnHandler         srh3;
        GetPreviousRequest::GetPreviousMsg  msg3( uut, payload3, srh3 );
        recordServer.postSync( msg3 );
        REQUIRE( payload3.m_success == true );
        REQUIRE( payload3.m_markerEntryRetreived.mediaOffset == actualEntrySize * 2 );
        REQUIRE( payload3.m_markerEntryRetreived.indexValue == 2 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );

        size_t idx = 0;
        appPayload.appSet( "" );
        GetByBufferIndexRequest::Payload             payload4( appPayload, idx );
        Cpl::Itc::SyncReturnHandler                  srh4;
        GetByBufferIndexRequest::GetByBufferIndexMsg msg4( uut, payload4, srh4 );
        recordServer.postSync( msg4 );
        REQUIRE( payload4.m_success == true );
        REQUIRE( payload4.m_markerEntryRetreived.mediaOffset == (actualEntrySize  * idx + actualEntrySize) );
        REQUIRE( payload4.m_markerEntryRetreived.indexValue == 1 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );
        REQUIRE( uut.getMaxIndex() == MAX_ENTRIES - 1 );

        uut.close();
        recordServer.close();
    }



    SECTION( "Clean-all" )
    {
        recordServer.open();
        uut.open();

        ClearAllEntriesRequest::Payload            payload;
        Cpl::Itc::SyncReturnHandler                srh;
        ClearAllEntriesRequest::ClearAllEntriesMsg msg( uut, payload, srh );
        recordServer.postSync( msg );

        uint32_t count;
        mp_elemCount_.read( count );
        REQUIRE( count == 0 );

        for ( size_t idx=0; idx <= entryRecord.getMaxIndex(); idx++ )
        {
            GetByBufferIndexRequest::Payload             payload4( appPayload, 1 );
            Cpl::Itc::SyncReturnHandler                  srh4;
            GetByBufferIndexRequest::GetByBufferIndexMsg msg4( uut, payload4, srh4 );
            recordServer.postSync( msg4 );
            REQUIRE( payload4.m_success == false );
        }

        uut.close();
        recordServer.close();
    }

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Api::sleep( 1000 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
