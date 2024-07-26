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
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Type/enum.h"
#include "Cpl/Logging/Api.h"
#include "Cpl/Logging/LogSink.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/System/Api.h"

#define SECT_     "_0test"

/// 
using namespace Cpl::Logging;


////////////////////////////////////////////////////////////////////////////////
/// Use anonymous namespace to make my class local-to-the-file in scope
namespace {

class MySink : public LogSink
{
public:
    ///
    MySink( Cpl::Dm::MailboxServer&                                     myMbox,
            Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T>&    incomingEntriesBuffer ) noexcept
        : LogSink( myMbox, incomingEntriesBuffer )
        , m_dispatchCount( 0 )
    {
    }

public:
    void dispatchLogEntry( Cpl::Logging::EntryData_T& src )
    {
        m_dispatchCount++;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("LOG: %llu, %lu:%u [%s]",
                                       src.timestamp,
                                       src.category,
                                       (unsigned) src.msgId,
                                       src.msgText) );
    }

public:
    int m_dispatchCount;
};

BETTER_ENUM( CategoryId, uint32_t
             , CRITICAL    = 0x00000001
             , WARNING     = 0x00000002
             , EVENT       = 0x00000004 );

BETTER_ENUM( CriticalMsg, uint16_t, OUT_OF_MEMORY, BOB_HAPPENED );
BETTER_ENUM( WarningMsg, uint16_t, BATTERY_LOW, CONNECTION_DROPPED, LOGGING_OVERFLOW );
BETTER_ENUM( EventMsg, uint16_t, ELVIS_HAS_LEFT_THE_BUILDING, SKY_IS_FALLING );

void logf( CriticalMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    vlogf<CategoryId, CriticalMsg>( CategoryId::CRITICAL, msgId, msgTextFormat, ap );
    va_end( ap );
}

void logf( WarningMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    vlogf<CategoryId, WarningMsg>( CategoryId::WARNING, msgId, msgTextFormat, ap );
    va_end( ap );
}

void logf( EventMsg msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    vlogf<CategoryId, EventMsg>( CategoryId::EVENT, msgId, msgTextFormat, ap );
    va_end( ap );
}

} // end namespace


// Allocate/create my Data model stuffs
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::Dm::Mp::Uint32       mp_fifoCount( modelDb_, "fifoCount" );

// Create the Logging FIFO
#define MAX_FIFO_ENTRIES    (5+1)
static Cpl::Logging::EntryData_T fifoMemory_[MAX_FIFO_ENTRIES];
static Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T> logFifo_( MAX_FIFO_ENTRIES, fifoMemory_, mp_fifoCount );


////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "LogSink" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    logFifo_.clearTheBuffer();
    mp_fifoCount.setInvalid();
    static Cpl::Dm::MailboxServer   t1Mbox;
    Cpl::System::Thread*            t1 = Cpl::System::Thread::create( t1Mbox, "T1" );
    MySink                          uut( t1Mbox, logFifo_ );

    SECTION( "drain" )
    {
        initialize( logFifo_,
                    CategoryId::WARNING,
                    (+CategoryId::WARNING)._to_string(),
                    WarningMsg::LOGGING_OVERFLOW,
                    (+WarningMsg::LOGGING_OVERFLOW)._to_string() );


        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );

        uut.open();
        for ( unsigned i=0; i < 10; i++ )
        {
            Cpl::System::Api::sleep( 100 );
            uint32_t count;
            if ( mp_fifoCount.read( count ) && count == 0 )
            {
                break;
            }
        }

        REQUIRE( uut.m_dispatchCount == 3 );
        uut.close();
    }

    t1Mbox.pleaseStop();
    Cpl::System::Api::sleep( 300 ); 
    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Api::sleep( 300 ); 

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );   REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

