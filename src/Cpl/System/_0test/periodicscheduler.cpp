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
#include "Cpl/System/Api.h"
#include "Cpl/System/PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


#define SECT_     "_0test"
/// 
using namespace Cpl::System;



////////////////////////////////////////////////////////////////////////////////

static inline void displayInfo( const char* label, unsigned count, ElapsedTime::Precision_T currentTick, ElapsedTime::Precision_T intervalTick, void* context )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("%-7s: %d, tick=%4d.%03d, interval=%4d.%03d, context=%p",
                                   label,
                                   count,
                                   currentTick.m_seconds,
                                   currentTick.m_thousandths,
                                   intervalTick.m_seconds,
                                   intervalTick.m_thousandths,
                                   context) );
}


static ElapsedTime::Precision_T appleLastCurrentTick_;
static ElapsedTime::Precision_T appleLastCurrentInterval_;
static void*                    appleLastContext_;
static unsigned                 appleCount_;

static void appleProcessInterval( ElapsedTime::Precision_T currentTick, ElapsedTime::Precision_T currentInterval, void* context )
{
    appleCount_++;
    appleLastCurrentTick_     = currentTick;
    appleLastCurrentInterval_ = currentInterval;
    appleLastContext_         = context;
}

static ElapsedTime::Precision_T orangeLastCurrentTick_;
static ElapsedTime::Precision_T orangeLastCurrentInterval_;
static void*                    orangeLastContext_;
static unsigned                 orangeCount_;

static void orangeProcessInterval( ElapsedTime::Precision_T currentTick, ElapsedTime::Precision_T currentInterval, void* context )
{
    orangeCount_++;
    orangeLastCurrentTick_     = currentTick;
    orangeLastCurrentInterval_ = currentInterval;
    orangeLastContext_         = context;
}

static ElapsedTime::Precision_T cherryLastCurrentTick_;
static ElapsedTime::Precision_T cherryLastCurrentInterval_;
static void*                    cherryLastContext_;
static unsigned                 cherryCount_;

static void cherryProcessInterval( ElapsedTime::Precision_T currentTick, ElapsedTime::Precision_T currentInterval, void* context )
{
    cherryCount_++;
    cherryLastCurrentTick_     = currentTick;
    cherryLastCurrentInterval_ = currentInterval;
    cherryLastContext_         = context;
}

static unsigned                 slippageCount_;
static ElapsedTime::Precision_T slippageLastCurrentTick_;
static ElapsedTime::Precision_T slippageLastMissedInterval_;
static void*                    slippageLastContext_;
static void reportSlippage( PeriodicScheduler::Interval_T& intervalThatSlipped, ElapsedTime::Precision_T currentTick, ElapsedTime::Precision_T missedInterval )
{
    slippageCount_++;
    slippageLastCurrentTick_     = currentTick;
    slippageLastMissedInterval_  = missedInterval;
    slippageLastContext_         = intervalThatSlipped.context;
    displayInfo( "SLIPPAGE", slippageCount_, slippageLastCurrentTick_, slippageLastMissedInterval_, slippageLastContext_ );
}

static unsigned                 startLoopCount_;
static ElapsedTime::Precision_T startLoopCountLastCurrentTick_;
static void loopStart( ElapsedTime::Precision_T currentTick )
{
    startLoopCount_++;
    startLoopCountLastCurrentTick_ = currentTick;
}

static unsigned                 endLoopCount_;
static ElapsedTime::Precision_T endLoopCountLastCurrentTick_;
static void loopEnd( ElapsedTime::Precision_T currentTick )
{
    endLoopCount_++;
    endLoopCountLastCurrentTick_ = currentTick;
}

static PeriodicScheduler::Interval_T intervals_[] =
{
    { appleProcessInterval, { 0,10 }, (void*) 0xCAFE },
    { orangeProcessInterval, { 0,20 }, (void*) 0xBEEF },
    { cherryProcessInterval, { 0,7 }, (void*) 0xFEED },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

static ElapsedTime::Precision_T currentTick_;
static ElapsedTime::Precision_T now()
{
    return currentTick_;
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PeriodicScheduler" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();
    appleCount_                    = 0;
    appleLastCurrentTick_          ={ 0,0 };
    appleLastCurrentInterval_      ={ 0,0 };
    appleLastContext_              = 0;
    orangeCount_                   = 0;
    orangeLastCurrentTick_         ={ 0,0 };
    orangeLastCurrentInterval_     ={ 0,0 };
    orangeLastContext_             = 0;
    cherryCount_                   = 0;
    cherryLastCurrentTick_         ={ 0,0 };
    cherryLastCurrentInterval_     ={ 0,0 };
    cherryLastContext_             = 0;
    slippageCount_                 = 0;
    slippageLastCurrentTick_       ={ 0,0 };
    slippageLastMissedInterval_    ={ 0,0 };
    slippageLastContext_           = 0;
    startLoopCount_                = 0;
    startLoopCountLastCurrentTick_ ={ 0,0 };
    endLoopCount_                  = 0;
    endLoopCountLastCurrentTick_   ={ 0,0 };

    currentTick_   ={ 0, 0 };

    SECTION( "no slippage reporting" )
    {
        PeriodicScheduler uut( intervals_, loopStart, loopEnd, nullptr, now );
        ElapsedTime::Precision_T intervalTime;

        currentTick_ ={ 0, 5 };
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.beginLoop();
        uut.executeScheduler();
        REQUIRE( appleCount_ == 0 );
        REQUIRE( orangeCount_ == 0 );
        REQUIRE( cherryCount_ == 0 );
        REQUIRE( startLoopCount_ == 1 );
        REQUIRE( startLoopCountLastCurrentTick_ == currentTick_ );
        REQUIRE( endLoopCount_ == 0 );
        currentTick_ += 5;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();

        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime ={ 0,10 };
        REQUIRE( appleCount_ == 1 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 0 );
        intervalTime ={ 0,7 };
        REQUIRE( cherryCount_ == 1 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        currentTick_ += 5;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();

        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 1 );
        REQUIRE( orangeCount_ == 0 );
        intervalTime ={ 0, 14 };
        REQUIRE( cherryCount_ == 2 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        currentTick_ += 5;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();

        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime ={ 0,20 };
        REQUIRE( appleCount_ == 2 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        intervalTime ={ 0,20 };
        REQUIRE( orangeCount_ == 1 );
        REQUIRE( orangeLastCurrentTick_ == currentTick_ );
        REQUIRE( orangeLastCurrentInterval_ == intervalTime );
        REQUIRE( cherryCount_ == 2 );

        currentTick_ += 1;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 2 );
        REQUIRE( orangeCount_ == 1 );
        intervalTime ={ 0, 21 };
        REQUIRE( cherryCount_ == 3 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        uut.endLoop();
        REQUIRE( startLoopCount_ == 1 );
        REQUIRE( endLoopCountLastCurrentTick_ == currentTick_ );
        REQUIRE( endLoopCount_ == 1 );
    }

    SECTION( "Slippage" )
    {
        PeriodicScheduler uut( intervals_, nullptr, nullptr, reportSlippage, now );
        ElapsedTime::Precision_T intervalTime;

        currentTick_ ={ 0, 5 };
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.beginLoop();
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 0 );
        REQUIRE( orangeCount_ == 0 );
        REQUIRE( cherryCount_ == 0 );
        REQUIRE( slippageCount_ == 0 );
        REQUIRE( startLoopCount_ == 0 );
        REQUIRE( endLoopCount_ == 0 );

        currentTick_ += 5;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 1 );
        REQUIRE( orangeCount_ == 0 );
        REQUIRE( cherryCount_ == 1 );
        REQUIRE( slippageCount_ == 0 );

        // Slip a single interval
        currentTick_ += 20;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime ={ 0,20 };
        REQUIRE( appleCount_ == 2 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 1 );
        intervalTime ={ 0,14 };
        REQUIRE( cherryCount_ == 2 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );
        REQUIRE( slippageCount_ == 2 );
        REQUIRE( slippageLastContext_ == (void*) 0xFEED );

        currentTick_ += 10;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime ={ 0,40 };
        REQUIRE( appleCount_ == 3 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 2 );
        intervalTime ={ 0, (40 / 7) * 7 };
        REQUIRE( cherryCount_ == 3 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );
        REQUIRE( slippageCount_ == 2 );

        // Slip multiple intervals
        currentTick_ += 30;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime ={ 0, 40 + 10 };
        REQUIRE( appleCount_ == 4 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        intervalTime ={ 0, (40 / 7 + 1) * 7 };
        REQUIRE( cherryCount_ == 4 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        // Verify interval boundaries after 'big' slippage
        currentTick_ += 10;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("== TICK: %04d.%03d", currentTick_.m_seconds, currentTick_.m_thousandths) );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime ={ 0, 80 };
        REQUIRE( appleCount_ == 5 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 4 );
        intervalTime ={ 0, (80 / 7) * 7 };
        REQUIRE( cherryCount_ == 5 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );
        REQUIRE( slippageCount_ == 4 );

        uut.endLoop();
        REQUIRE( startLoopCount_ == 0 );
        REQUIRE( endLoopCount_ == 0 );
    }

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}
