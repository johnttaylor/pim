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


#include "AirFilterMonitor.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"
#include "Cpl/Dm/ModelPoint.h"

#define SECT_   "Storm::Component::AirFilterMonitor"


/// Namespaces
using namespace Storm::Component;



///////////////////////////////
AirFilterMonitor::AirFilterMonitor( struct AirFilterMonitor::Input_T ins, struct AirFilterMonitor::Output_T outs )
    : m_in( ins )
    , m_out( outs )
{
    CPL_SYSTEM_ASSERT( m_in.maxAirFilterHours );
    CPL_SYSTEM_ASSERT( m_in.airFilterOperationTime );
    CPL_SYSTEM_ASSERT( m_in.airFilterAlert );
    CPL_SYSTEM_ASSERT( m_in.vOutputs );
    CPL_SYSTEM_ASSERT( m_out.airFilterAlert );
    CPL_SYSTEM_ASSERT( m_out.airFilterOperationTime );
}

bool AirFilterMonitor::start( Cpl::System::ElapsedTime::Precision_T& intervalTime )
{
    // House keeping
    m_blowerWasOn = false;

    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool AirFilterMonitor::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                                Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing
    //--------------------------------------------------------------------------

    // Get my                             inputs
    Storm::Dm::MpSimpleAlarm::Data        alert         ={ 0, };
    uint32_t                              maxHours      = 0;
    Cpl::System::ElapsedTime::Precision_T elapsedTime;
    Storm::Type::VirtualOutputs_T         outputs       ={ 0, };
    int8_t                                validAlert    = m_in.airFilterAlert->read( alert );
    int8_t                                validHours    = m_in.maxAirFilterHours->read( maxHours );
    int8_t                                validElapsed  = m_in.airFilterOperationTime->read( elapsedTime );
    int8_t                                validOutputs  = m_in.vOutputs->read( outputs );

    if ( validAlert == false ||
         validHours == false ||
         validElapsed == false ||
         validOutputs == false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "AirFilterMonitor::execute. One or more invalid MPs (alert=%d, hours=%d, elapsed=%d vOutput=%d", validAlert, validHours, validElapsed, validOutputs ) );

        // Do nothing if I do not have all valid inputs
        return true;
    }


    //--------------------------------------------------------------------------
    // Algorithm processing
    //--------------------------------------------------------------------------

    // Skip if the Alert has already been raised
    if ( alert.active == false )
    {
        // Is the Indoor blower on?
        if ( outputs.indoorFan != STORM_DM_MP_VIRTUAL_OUTPUTS_OFF || outputs.indoorFanCont != STORM_DM_MP_VIRTUAL_OUTPUTS_OFF )
        {
            // Capture the turn-on-edge time for the blower
            if ( m_blowerWasOn == false )
            {
                m_blowerWasOn  = true;
                m_markBlowerOn = currentInterval;
            }

            // Accumulate the operation time
            Cpl::System::ElapsedTime::Precision_T delta = Cpl::System::ElapsedTime::deltaPrecision( m_markBlowerOn, currentInterval );
            elapsedTime   += delta;
            m_markBlowerOn = currentInterval;

            // Has the time limit expired?
            Cpl::System::ElapsedTime::Precision_T limit ={ maxHours * 60 * 60, 0 };
            if ( elapsedTime >= limit )
            {
                // Raise the alert
                alert.active   = true;
                alert.acked    = false;
                alert.critical = false;

                // Reset my blower state flag so that when the Alert is cleared - we restart correctly
                m_blowerWasOn = false;

                // Reset the elapsed time when the Alert is declared
                elapsedTime ={ 0, 0 };
            }
        }

        // Update my outputs
        m_out.airFilterAlert->write( alert );
        m_out.airFilterOperationTime->write( elapsedTime );
    }

    //--------------------------------------------------------------------------
    // Post-Algorithm processing
    //--------------------------------------------------------------------------

    // If I get here -->everything worked!
    return true;
}


