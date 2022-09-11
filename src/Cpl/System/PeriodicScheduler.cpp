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

#include "PeriodicScheduler.h"
#include "Trace.h"
#include "FatalError.h"
#include "Assert.h"

#define SECT_ "Cpl::System"


///
using namespace Cpl::System;

/////////////////////
PeriodicScheduler::PeriodicScheduler( Interval_T           intervals[],
                                      Hook_T               beginThreadProcessing,
                                      Hook_T               endThreadProcessing,
                                      ReportSlippageFunc_T slippageFunc,
                                      NowFunc_T            nowFunc )
    : m_intervals( intervals )
    , m_reportSlippage( slippageFunc )
    , m_nowFunc( nowFunc )
    , m_beginThreadFunc( beginThreadProcessing )
    , m_endThreadFunc( endThreadProcessing )
    , m_firstExecution( true )
{
    CPL_SYSTEM_ASSERT( intervals );
    CPL_SYSTEM_ASSERT( nowFunc );
}

void PeriodicScheduler::beginLoop()
{
    if ( m_beginThreadFunc )
    {
        (m_beginThreadFunc) ((m_nowFunc) ());
    }
}

void PeriodicScheduler::endLoop()
{
    if ( m_endThreadFunc )
    {
        (m_endThreadFunc) ((m_nowFunc) ());
    }
}

/////////////////////
bool PeriodicScheduler::executeScheduler()
{
    bool         atLeastOne   = false;
    Interval_T*  interval     = m_intervals;

    // Scan all intervals
    while ( interval && interval->callbackFunc != 0 )
    {
        // Get the current time
        ElapsedTime::Precision_T currentTick = (m_nowFunc) ();

        // Initialize the interval (but only once)
        if ( m_firstExecution )
        {
            setTimeMarker( *interval, currentTick );
        }

        // Has the interval expired?
        if ( ElapsedTime::expiredPrecision( interval->timeMarker, interval->duration, currentTick ) )
        {
            atLeastOne            = true;
            interval->timeMarker += interval->duration;
            (interval->callbackFunc)(currentTick, interval->timeMarker, interval->context);

            // Check for slippage
            if ( ElapsedTime::expiredPrecision( interval->timeMarker, interval->duration, currentTick ) )
            {
                // Report the slippage to the application
                if ( m_reportSlippage )
                {
                    (m_reportSlippage) (*interval, currentTick, interval->timeMarker);
                }

                // Re-sync the most recent past interval boundary based on the actual time.
                // Note: This operation only has a 'effect' if the slipped time is greater than 2 duration times
                setTimeMarker( *interval, currentTick );
            }
        }

        // Get the next interval
        interval++;
    }

    // Clear flag now that we have properly initialized each interval
    m_firstExecution = false;

    // Return result;
    return atLeastOne;
}



void PeriodicScheduler::setTimeMarker( Interval_T& interval, ElapsedTime::Precision_T currentTick ) noexcept
{
    // Make sure there is no divide by zero error
    uint64_t duration = interval.duration.asFlatTime();
    if ( duration == 0 )
    {
        interval.timeMarker = currentTick;
        return;
    }

    // Round down to the nearest interval boundary
    interval.timeMarker.setFlatTime( (currentTick.asFlatTime() / duration) * duration );
}
