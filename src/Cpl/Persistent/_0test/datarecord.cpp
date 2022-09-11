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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Persistent/DataRecord.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/CrcChunk.h"
#include "Cpl/Persistent/FileAdapter.h"
#include "Cpl/Io/File/Api.h"


#define SECT_   "_0test"

#define FILE_NAME_REGION1   "region1.nvram"

using namespace Cpl::Persistent;

namespace {

class MyRecord : public DataRecord
{
public:
    MyRecord( Cpl::Persistent::Chunk& chunkHandler ) noexcept
        : DataRecord( chunkHandler )
        , m_noValidData( 0 )
        , m_getDataCount( 0 )
        , m_putDataCount( 0 )
        , m_noValidResult( true )
    {
        memset( m_data, 0, sizeof( m_data ) );
    }

    bool processNoValidData() noexcept
    {
        memset( m_data, 0, sizeof( m_data ) );
        m_noValidData++;
        return m_noValidResult;
    }

    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        m_getDataCount++;
        if ( maxDstLen >= sizeof( m_data ) )
        {
            memcpy( dst, m_data, sizeof( m_data ) );
            return sizeof( m_data );
        }

        return 0;
    }

    bool putData( const void* src, size_t srcLen ) noexcept
    {
        m_putDataCount++;
        if ( srcLen <= sizeof( m_data ) )
        {
            memcpy( m_data, src, sizeof( m_data ) );
            return true;
        }

        return false;
    }

    int m_noValidData;
    int m_getDataCount;
    int m_putDataCount;
    int m_data[4];
    bool m_noValidResult;
};

}; // end anonymous namespace


static Cpl::Persistent::FileAdapter fd1( FILE_NAME_REGION1, 0, 128 );
static Cpl::Persistent::CrcChunk chunk( fd1 );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "datarecord" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "DATARECORD Test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    MyRecord uut( chunk );
    int expectedData[4] = { 0, };
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
        REQUIRE( uut.m_noValidData == 0 );
        REQUIRE( memcmp( expectedData, uut.m_data, sizeof( uut.m_data ) ) == 0 );

        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidData == 1 );
        REQUIRE( memcmp( expectedData, uut.m_data, sizeof( uut.m_data ) ) == 0 );

        uut.m_noValidResult = false;
        REQUIRE( uut.readFromMedia() == false );
        REQUIRE( uut.m_getDataCount == 0 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidData == 2 );
        REQUIRE( memcmp( expectedData, uut.m_data, sizeof( uut.m_data ) ) == 0 );

        // Write some data
        uut.m_data[0] = 1;
        uut.m_data[1] = 2;
        uut.m_data[2] = 3;
        uut.m_data[3] = 4;
        REQUIRE( uut.writeToMedia() == true );
        REQUIRE( uut.m_getDataCount == 1 );
        REQUIRE( uut.m_putDataCount == 0 );
        REQUIRE( uut.m_noValidData == 2 );

        // Read back the data
        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_getDataCount == 1 );
        REQUIRE( uut.m_putDataCount == 1 );
        REQUIRE( uut.m_noValidData == 2 );
        expectedData[0] = 1;
        expectedData[1] = 2;
        expectedData[2] = 3;
        expectedData[3] = 4;
        REQUIRE( memcmp( expectedData, uut.m_data, sizeof( uut.m_data ) ) == 0 );

        recordServer.close();
    }

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Api::sleep( 1000 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
