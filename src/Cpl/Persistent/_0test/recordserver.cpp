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
#include "Cpl/Persistent/RecordServer.h"

#define SECT_ "_0test"

using namespace Cpl::Persistent;

class MyRecord : public Record
{
public:
    int m_startCount;
    int m_stopCount;

    MyRecord() :m_startCount( 0 ), m_stopCount( 0 ) {}
    
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept
    {
        m_startCount++;
    }

    void stop() noexcept
    {
        m_stopCount++;
    }
};


static MyRecord record1_;
static MyRecord record2_;
static Record*  records_[] ={ &record1_, &record2_, 0 };

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "recordserver" )
{
    CPL_SYSTEM_TRACE_SCOPE( SECT_, "RECORD_SERVER-OPEN test" );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    
    RecordServer uut( records_ );
    Cpl::System::Thread* t1 = Cpl::System::Thread::create( uut, "UUT" );
    REQUIRE( t1 );

    REQUIRE( record1_.m_startCount == 0 );
    REQUIRE( record1_.m_stopCount == 0 );
    REQUIRE( record2_.m_startCount == 0 );
    REQUIRE( record2_.m_stopCount == 0 );

    uut.open();
    REQUIRE( record1_.m_startCount == 1 );
    REQUIRE( record1_.m_stopCount == 0 );
    REQUIRE( record2_.m_startCount == 1 );
    REQUIRE( record2_.m_stopCount == 0 );

    uut.open();
    REQUIRE( record1_.m_startCount == 1 );
    REQUIRE( record1_.m_stopCount == 0 );
    REQUIRE( record2_.m_startCount == 1 );
    REQUIRE( record2_.m_stopCount == 0 );

    uut.close();
    REQUIRE( record1_.m_startCount == 1 );
    REQUIRE( record1_.m_stopCount == 1 );
    REQUIRE( record2_.m_startCount == 1 );
    REQUIRE( record2_.m_stopCount == 1 );

    uut.close();
    REQUIRE( record1_.m_startCount == 1 );
    REQUIRE( record1_.m_stopCount == 1 );
    REQUIRE( record2_.m_startCount == 1 );
    REQUIRE( record2_.m_stopCount == 1 );

    uut.open();
    REQUIRE( record1_.m_startCount == 2 );
    REQUIRE( record1_.m_stopCount == 1 );
    REQUIRE( record2_.m_startCount == 2 );
    REQUIRE( record2_.m_stopCount == 1 );

    uut.close();
    REQUIRE( record1_.m_startCount == 2 );
    REQUIRE( record1_.m_stopCount == 2 );
    REQUIRE( record2_.m_startCount == 2 );
    REQUIRE( record2_.m_stopCount == 2 );

    Cpl::System::Thread::destroy( *t1 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
