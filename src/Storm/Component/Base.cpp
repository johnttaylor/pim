/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/


#include "Base.h"


/// Namespaces
using namespace Storm::Component;



///////////////////////////////
Base::Base( void )
    :m_slipCounter( 0 )
    , m_timeMarkValid( false )
    , m_started( false )
{
}

void Base::setTimeMarker( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    // Round DOWN to the nearest 'interval' boundary
    // NOTE: By setting the initial time mark to the an "interval boundary" 
    //       I can ensure that ALL components with the same interval value
    //       execute in the same pass of the main() loop.  One side effect
    //       of this algorithm is the first execution interval will NOT be
    //       accurate (i.e. will be something less than 'm_interval'). 
    m_timeMark.m_seconds     = m_interval.m_seconds == 0 ? currentTick.m_seconds : ( currentTick.m_seconds / m_interval.m_seconds ) * m_interval.m_seconds;
    m_timeMark.m_thousandths = m_interval.m_thousandths == 0 ? currentTick.m_thousandths : ( currentTick.m_thousandths / m_interval.m_thousandths ) * m_interval.m_thousandths;
}

///////////////////////////////
bool Base::doWork( bool enabled, Cpl::System::ElapsedTime::Precision_T currentTick )
{
    // Check if my interval time has expired (or first time)
    if ( !m_timeMarkValid || Cpl::System::ElapsedTime::expiredPrecision( m_timeMark, m_interval, currentTick ) )
    {
        // Calculate the first/initial interval boundary
        if ( !m_timeMarkValid )
        {
            setTimeMarker( currentTick );
        }

        // Update my time marker and MAINTAIN absolute interval boundaries.  
        else
        {
            m_timeMark += m_interval;
        }

        // Detect when I am not meeting my interval time, i.e. not able to call 
        // the do() method at least once every 'interval'.  Typically this is 
        // BAD - but not bad enough to abort the application.
        if ( m_timeMarkValid && Cpl::System::ElapsedTime::expiredPrecision( m_timeMark, m_interval, currentTick ) )
        {
            m_slipCounter++;
            manageSlippage( currentTick );
            setTimeMarker( currentTick );       // Adjust my time marker for the 'real time'
        }

        // Set my state to I have executed (or about to) the first 'interval'
        m_timeMarkValid = true;

        // Execute the Component
        if ( m_started && enabled )
        {
            return execute( currentTick, m_timeMark );
        }
    }


    // If I get here I am either NOT at an interval boundary/marker or I am disabled
    return true;
}


///////////////////////////////
bool Base::start( Cpl::System::ElapsedTime::Precision_T intervalTime )
{
    m_interval      = intervalTime;
    m_timeMarkValid = false;
    m_slipCounter   = 0;
    m_started       = true;
    return true;
}


bool Base::stop( void )
{
    m_started = false;
    return true;
}


///////////////////////////////
void Base::manageSlippage( Cpl::System::ElapsedTime::Precision_T currentTick )
{
}
