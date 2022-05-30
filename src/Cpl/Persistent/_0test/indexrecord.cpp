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
#include "Cpl/Persistent/IndexRecord.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/CrcChunk.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Io/File/Api.h"


#define SECT_   "_0test"

#define FILE_NAME_REGION1   "region1.nvram"

using namespace Cpl::Persistent;

namespace {

class MyRecord : public IndexRecord
{
public:
    MyRecord( Cpl::Persistent::Chunk& chunkHandler ) noexcept
        : IndexRecord( chunkHandler )
        , m_noValidDataCount( 0 )
        , m_getDataCount( 0 )
        , m_putDataCount( 0 )
        , m_lastLatest( 0 )
        , m_lastLatestIndex( 0 )
        , m_failProcessNoValidData( false )
    {
    }

    bool processNoValidData() noexcept
    {
        m_noValidDataCount++;
        if ( m_failProcessNoValidData )
        {
            return false;
        }

        bool result       = IndexRecord::processNoValidData();
        m_lastLatest      = m_latest;
        m_lastLatestIndex = m_latestIndex;
        return result;
    }

    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        m_getDataCount++;
        return IndexRecord::getData( dst, maxDstLen );
    }

    bool putData( const void* src, size_t srcLen ) noexcept
    {
        m_putDataCount++;
        bool result       = IndexRecord::putData( src, srcLen );
        m_lastLatest      = m_latest;
        m_lastLatestIndex = m_latestIndex;
        return result;
    }

    void setLatestOffset( size_t offset, uint64_t indexValue ) noexcept
    {
        IndexRecord::setLatestOffset( offset, indexValue );
        m_lastLatest      = m_latest;
        m_lastLatestIndex = indexValue;
    }



    int       m_noValidDataCount;
    int       m_getDataCount;
    int       m_putDataCount;
    size_t    m_lastLatest;
    uint64_t  m_lastLatestIndex;
    bool      m_failProcessNoValidData;
};

}; // end anonymous namespace


static Cpl::Persistent::FileAdapter fd1( FILE_NAME_REGION1, 0, 128 );
static Cpl::Persistent::CrcChunk chunk( fd1 );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "indexrecord" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "INDEXRECORD Test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    MyRecord uut( chunk );
    Cpl::Persistent::Record* records[2] = { &uut, 0 };
    Cpl::Persistent::RecordServer recordServer( records );
    Cpl::System::Thread* t1 = Cpl::System::Thread::create( recordServer, "UUT" );
    REQUIRE( t1 );


    SECTION( "no persistent data/write" )
    {
        // Delete files
        Cpl::Io::File::Api::remove( FILE_NAME_REGION1 );

        // No persistent data
        recordServer.open();
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 0 );

        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 1 );
        REQUIRE( uut.m_lastLatest == 0 );
        REQUIRE( uut.m_lastLatestIndex == 0 );

        uut.m_failProcessNoValidData = true;
        REQUIRE( uut.readFromMedia() == false );
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 2 );

        // Write some data
        uut.setLatestOffset( 1, 11 );
        REQUIRE( uut.writeToMedia() == true );
        REQUIRE( uut.m_getDataCount == 1 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 2 );
        REQUIRE( uut.m_lastLatest == 1 );
        REQUIRE( uut.m_lastLatestIndex == 11 );

        recordServer.close();
    }

    SECTION( "read data" )
    {
        // Existing persistent data
        recordServer.open();
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 0 );

        // Read back the data
        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 1 );
        REQUIRE( uut.m_noValidDataCount == 0 );
        REQUIRE( uut.m_lastLatest == 1 );
        REQUIRE( uut.m_lastLatestIndex == 11 );

        recordServer.close();
    }

    SECTION( "invalidate offsets" )
    {
        // Start the UUT
        recordServer.open();
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 0 );

        size_t dummy[2+2*2];
        size_t offset = 0;
        uint64_t indexValue;
        REQUIRE( uut.getLatestOffset( offset, indexValue ) == false );
        REQUIRE( uut.getData( dummy, sizeof( dummy ) ) == 0 );
        uut.setLatestOffset( offset, 12 );
        REQUIRE( uut.getLatestOffset( offset, indexValue ) == true );
        REQUIRE( uut.m_lastLatestIndex == 12 );

        recordServer.close();
    }

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Api::sleep( 1000 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
