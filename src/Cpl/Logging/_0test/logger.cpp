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
#include "Cpl/Dm/ModelDatabase.h"

#define SECT_     "_0test"

/// 
using namespace Cpl::Logging;


////////////////////////////////////////////////////////////////////////////////
/// Use anonymous namespace to make my class local-to-the-file in scope
namespace {

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

static CplLoggingTime_T nowValue_;
CplLoggingTime_T Cpl::Logging::now() noexcept
{
    return nowValue_++;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "logger" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    logFifo_.clearTheBuffer();
    mp_fifoCount.setInvalid();

    SECTION( "happy path" )
    {
        initialize( logFifo_,
                    CategoryId::WARNING,
                    (+CategoryId::WARNING)._to_string(),
                    WarningMsg::LOGGING_OVERFLOW,
                    (+WarningMsg::LOGGING_OVERFLOW)._to_string() );
                    

        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );

        uint32_t logCount;
        bool     valid = mp_fifoCount.read( logCount );
        REQUIRE( valid );
        REQUIRE( logCount == 3 );

        EntryData_T logEntry;
        bool result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::CRITICAL );
        REQUIRE( logEntry.msgId == CriticalMsg::BOB_HAPPENED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::WARNING );
        REQUIRE( logEntry.msgId == WarningMsg::CONNECTION_DROPPED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::EVENT );
        REQUIRE( logEntry.msgId == EventMsg::ELVIS_HAS_LEFT_THE_BUILDING );

        result = logFifo_.remove( logEntry );
        REQUIRE( result == false);

        shutdown();
    }

    SECTION( "queue overflow" )
    {
        initialize( logFifo_,
                    CategoryId::WARNING,
                    (+CategoryId::WARNING)._to_string(),
                    WarningMsg::LOGGING_OVERFLOW,
                    (+WarningMsg::LOGGING_OVERFLOW)._to_string() );


        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );
        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );
        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );

        uint32_t logCount;
        bool     valid = mp_fifoCount.read( logCount );
        REQUIRE( valid );
        REQUIRE( logCount == MAX_FIFO_ENTRIES-1 );

        EntryData_T logEntry;
        bool result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::CRITICAL );
        REQUIRE( logEntry.msgId == CriticalMsg::BOB_HAPPENED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::WARNING );
        REQUIRE( logEntry.msgId == WarningMsg::CONNECTION_DROPPED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::EVENT );
        REQUIRE( logEntry.msgId == EventMsg::ELVIS_HAS_LEFT_THE_BUILDING );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::CRITICAL );
        REQUIRE( logEntry.msgId == CriticalMsg::BOB_HAPPENED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::WARNING );
        REQUIRE( logEntry.msgId == WarningMsg::CONNECTION_DROPPED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result == false );


        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );
        valid = mp_fifoCount.read( logCount );
        REQUIRE( valid );
        REQUIRE( logCount == 2 );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::WARNING );
        REQUIRE( logEntry.msgId == WarningMsg::LOGGING_OVERFLOW );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::EVENT );
        REQUIRE( logEntry.msgId == EventMsg::ELVIS_HAS_LEFT_THE_BUILDING );

        result = logFifo_.remove( logEntry );
        REQUIRE( result == false );

        shutdown();
    }

    SECTION( "mask" )
    {
        initialize( logFifo_,
                    CategoryId::WARNING,
                    (+CategoryId::WARNING)._to_string(),
                    WarningMsg::LOGGING_OVERFLOW,
                    (+WarningMsg::LOGGING_OVERFLOW)._to_string() );


        setCategoryMask( CategoryId::EVENT );

        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );

        uint32_t logCount;
        bool     valid = mp_fifoCount.read( logCount );
        REQUIRE( valid );
        REQUIRE( logCount == 1 );

        EntryData_T logEntry;
        bool result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::EVENT );
        REQUIRE( logEntry.msgId == EventMsg::ELVIS_HAS_LEFT_THE_BUILDING );

        result = logFifo_.remove( logEntry );
        REQUIRE( result == false );

        enableCategory( CategoryId::WARNING );
        uint32_t mask = getCategoryEnabledMask();
        REQUIRE( mask == (CategoryId::WARNING | CategoryId::EVENT ) );

        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::WARNING );
        REQUIRE( logEntry.msgId == WarningMsg::CONNECTION_DROPPED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result == false );

        disableCategory( CategoryId::EVENT );
        logf( CriticalMsg::BOB_HAPPENED, "dang that bob!" );
        logf( EventMsg::ELVIS_HAS_LEFT_THE_BUILDING, "So long and thanks for the music" );
        logf( WarningMsg::CONNECTION_DROPPED, "status=%d", true );

        result = logFifo_.remove( logEntry );
        REQUIRE( result );
        REQUIRE( logEntry.category == CategoryId::WARNING );
        REQUIRE( logEntry.msgId == WarningMsg::CONNECTION_DROPPED );

        result = logFifo_.remove( logEntry );
        REQUIRE( result == false );

        shutdown();
    }
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

