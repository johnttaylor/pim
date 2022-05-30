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
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Persistent/IndexedEntryRecord.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/CrcChunk.h"
#include "Cpl/Persistent/MirroredChunk.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/Mp/Uint64.h"

#include <stdio.h>

#define SECT_   "_0test"

#define FILE_NAME_INDEX_REGION1   "index1.nvram"
#define FILE_NAME_INDEX_REGION2   "index2.nvram"
#define FILE_NAME_ENTRY_REGION    "entries.nvram"

#define ENTRY_MAX_SIZE              7   
#define ENTRY_REGION_SIZE           512

#define ENTRY1  "Hello"
#define ENTRY2  "World"
#define ENTRY3  "GoodBy"

using namespace Cpl::Persistent;

namespace {

class MyUut : public IndexedEntryRecord
{
public:
    MyUut( Chunk&                  entryChunkHandler,
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
        return ENTRY_MAX_SIZE + getMetadataLength() + m_chunkHandler.getMetadataLength();
    }
};

class AppEntryPayload : public Payload
{
public:
    AppEntryPayload() { memset( m_buffer, 0, ENTRY_MAX_SIZE ); }

public:
    void appSet( const char* newValue )
    {
        m_buffer[0] = '\0';
        strncat( m_buffer, newValue, ENTRY_MAX_SIZE - 1 );
    }

public:
    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        REQUIRE( maxDstLen >= ENTRY_MAX_SIZE );
        memcpy( dst, m_buffer, ENTRY_MAX_SIZE );
        return ENTRY_MAX_SIZE;
    }

    bool putData( const void* src, size_t srcLen ) noexcept
    {
        REQUIRE( ENTRY_MAX_SIZE <= srcLen );
        memcpy( m_buffer, src, ENTRY_MAX_SIZE );
        return true;
    }

public:
    char m_buffer[ENTRY_MAX_SIZE];
};

}; // end anonymous namespace

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Cpl::Dm::Mp::Uint64    mp_index_( modelDb_, "index" );


static Cpl::Persistent::FileAdapter     indexFd1( FILE_NAME_INDEX_REGION1, 0, 128 );
static Cpl::Persistent::FileAdapter     indexFd2( FILE_NAME_INDEX_REGION2, 0, 128 );
static Cpl::Persistent::FileAdapter     entriesFd( FILE_NAME_ENTRY_REGION, 0, ENTRY_REGION_SIZE );
static Cpl::Persistent::MirroredChunk   indexRecChunk( indexFd1, indexFd2 );
static Cpl::Persistent::CrcChunk        entriesChunk( entriesFd );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "IndexedEntryRecord" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "INDEXED-ENTRY-RECORD Test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    mp_index_.setInvalid();
    AppEntryPayload    appPayload;
    IndexRecord        indexRecord( indexRecChunk );
    MyUut              uut( entriesChunk, ENTRY_MAX_SIZE, entriesFd, indexRecord, mp_index_ );
    Cpl::Persistent::Record* records[2] = { &uut, 0 };
    Cpl::Persistent::RecordServer recordServer( records );
    Cpl::System::Thread* t1 = Cpl::System::Thread::create( recordServer, "UUT" );
    REQUIRE( t1 );

    size_t actualEntrySize = uut.getTotalEntryLength();

    SECTION( "no persistent data" )
    {
        uint64_t indexVal;

        // Delete files
        Cpl::Io::File::Api::remove( FILE_NAME_INDEX_REGION1 );
        Cpl::Io::File::Api::remove( FILE_NAME_INDEX_REGION2 );
        Cpl::Io::File::Api::remove( FILE_NAME_ENTRY_REGION );

        // No persistent data
        recordServer.open();

        IndexedEntryReader::EntryMarker_T marker;
        bool result = uut.getLatest( appPayload, marker );
        REQUIRE( result == false );

        result = indexRecord.getLatestOffset( marker.mediaOffset, marker.indexValue );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == 0 );
        REQUIRE( marker.indexValue == 0 );

        // Write 3 entries
        appPayload.appSet( ENTRY1 );
        result = uut.addEntry( appPayload );
        REQUIRE( result == true );
        REQUIRE( mp_index_.read( indexVal ) );
        REQUIRE( indexVal == 1 );
        appPayload.appSet( ENTRY2 );
        result = uut.addEntry( appPayload );
        REQUIRE( result == true );
        REQUIRE( mp_index_.read( indexVal ) );
        REQUIRE( indexVal == 2 );
        appPayload.appSet( ENTRY3 );
        result = uut.addEntry( appPayload );
        REQUIRE( result == true );
        REQUIRE( mp_index_.read( indexVal ) );
        REQUIRE( indexVal == 3 );

        result = indexRecord.getLatestOffset( marker.mediaOffset, marker.indexValue );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );
        REQUIRE( marker.indexValue == 3 );

        recordServer.close();
    }

    SECTION( "startup - with data" )
    {
        uint64_t indexVal;

        recordServer.open();

        IndexedEntryReader::EntryMarker_T marker;
        bool result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );
        REQUIRE( marker.indexValue == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );
        result = indexRecord.getLatestOffset( marker.mediaOffset, marker.indexValue );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );
        REQUIRE( marker.indexValue == 3 );

        REQUIRE( mp_index_.read( indexVal ) );
        REQUIRE( indexVal == 3 );

        recordServer.close();
    }

    SECTION( "Corrupt index" )
    {
        // Delete files
        Cpl::Io::File::Api::remove( FILE_NAME_INDEX_REGION1 );
        Cpl::Io::File::Api::remove( FILE_NAME_INDEX_REGION2 );

        recordServer.open();

        IndexedEntryReader::EntryMarker_T marker;
        bool result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.indexValue == 3 );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );

        recordServer.close();
    }

    SECTION( "Walk-next/prev" )
    {
        recordServer.open();

        IndexedEntryReader::EntryMarker_T marker;
        bool result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );
        REQUIRE( marker.indexValue == 3 );

        appPayload.appSet( "<none>" );
        IndexedEntryReader::EntryMarker_T marker2;
        result = uut.getNext( marker.indexValue, marker, appPayload, marker2 );
        REQUIRE( result == false );

        appPayload.appSet( "<none>" );
        result = uut.getPrevious( marker.indexValue, marker, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );
        uint64_t timestampEntry2 = marker.indexValue;

        appPayload.appSet( "<none>" );
        result = uut.getPrevious( marker.indexValue, marker, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );

        appPayload.appSet( "<none>" );
        result = uut.getPrevious( marker.indexValue, marker, appPayload, marker );
        REQUIRE( result == false );

        appPayload.appSet( "<none>" );
        result = uut.getNext( timestampEntry2, marker, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        recordServer.close();
    }

    SECTION( "Walk by index" )
    {
        recordServer.open();

        IndexedEntryReader::EntryMarker_T marker;
        bool result = uut.getByBufferIndex( 2, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        appPayload.appSet( "<none>" );
        result = uut.getByBufferIndex( 0, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );

        appPayload.appSet( "<none>" );
        result = uut.getByBufferIndex( 3, appPayload, marker );
        REQUIRE( result == false );

        appPayload.appSet( "<none>" );
        result = uut.getByBufferIndex( 1, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );

        for ( size_t idx=0; idx <= uut.getMaxIndex() + 1; idx++ )
        {
            IndexedEntryReader::EntryMarker_T marker;
            bool expectedResult = idx < 3 ? true : false;
            REQUIRE( uut.getByBufferIndex( idx, appPayload, marker ) == expectedResult );
        }
        recordServer.close();
    }


    SECTION( "Clean-all" )
    {
        recordServer.open();

        REQUIRE( uut.clearAllEntries() == true );

        for ( size_t idx=0; idx <= uut.getMaxIndex(); idx++ )
        {
            IndexedEntryReader::EntryMarker_T marker;
            REQUIRE( uut.getByBufferIndex( idx, appPayload, marker ) == false );
        }

        recordServer.close();
    }

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Api::sleep( 1000 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
