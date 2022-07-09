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


#include "PiPreProcess.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"
#include "Cpl/Math/real.h"


#define SECT_               "Storm::Component::PiPreProcess"

#define INVALID_SETPOINT_   9999.0



/// Namespaces
using namespace Storm::Component;


///////////////////////////////
PiPreProcess::PiPreProcess( struct Input_T ins, struct Output_T outs )
    : m_in( ins )
    , m_out( outs )
{
    CPL_SYSTEM_ASSERT( m_in.activeIdt );
    CPL_SYSTEM_ASSERT( m_in.operatingModeChange );
    CPL_SYSTEM_ASSERT( m_in.setpoints );
    CPL_SYSTEM_ASSERT( m_in.systemConfig );
    CPL_SYSTEM_ASSERT( m_out.activeSetpoint );
    CPL_SYSTEM_ASSERT( m_out.idtDeltaError );
    CPL_SYSTEM_ASSERT( m_out.setpointChanged );
    CPL_SYSTEM_ASSERT( m_out.setpointDelta );
}


bool PiPreProcess::start( Cpl::System::ElapsedTime::Precision_T& intervalTime )
{
    // Initialize my data
    m_prevActiveSetpoint = INVALID_SETPOINT_;

    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool PiPreProcess::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval
)
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing   
    //--------------------------------------------------------------------------

    // Get my inputs
    float                                 idt               = 0.0F; // Default to 'bad value'
    float                                 coolSetpoint      = 0.0F;
    float                                 heatSetpoint      = 0.0F;
    bool                                  modeChanged       = false;
    Storm::Type::SystemConfig_T           sysCfg            ={ 0, };
    int8_t                                validIdt          = m_in.activeIdt->read( idt );
    int8_t                                validModeChanged  = m_in.operatingModeChange->read( modeChanged );
    int8_t                                validSetpoints    = m_in.setpoints->read( coolSetpoint, heatSetpoint );
    int8_t                                validSystem       = m_in.systemConfig->read( sysCfg );
    if ( validIdt == false ||
         validModeChanged == false ||
         validSetpoints == false ||
         validSystem == false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("PiPreProcess::execute. One or more invalid MPs (idt=%d, modechg=%d,setpts=%d, system=%d", validIdt, validModeChanged, validSetpoints, validSystem) );

        // Force the operating mode to off if I am missing one or more inputs values
        sysCfg.currentOpMode = Storm::Type::OperatingMode::eOFF;
    }

    // Default the output value(s)
    bool  setpointChanged   = false;
    float deltaIdtError     = 0;
    float setpointDelta     = 0;
    float newActiveSetpoint = coolSetpoint;

    //--------------------------------------------------------------------------
    // Algorithm processing
    //--------------------------------------------------------------------------

    // Use the current operating mode to calculate the delta error
    switch ( sysCfg.currentOpMode )
    {
        // COOLING
    case Storm::Type::OperatingMode::eCOOLING:
        deltaIdtError = idt - coolSetpoint;
        break;

        // HEATING
    case Storm::Type::OperatingMode::eHEATING:
    case Storm::Type::OperatingMode::eID_HEATING:
        newActiveSetpoint = heatSetpoint;
        deltaIdtError     = heatSetpoint - idt;
        break;

        // OFF mode (and any invalid mode settings)
    default:
        newActiveSetpoint = INVALID_SETPOINT_;
        break;
    }

    // Trap a change in the active setpoint (but NOT when there is a mode change)
    if ( modeChanged == false )
    {
        setpointDelta = fabs( newActiveSetpoint - m_prevActiveSetpoint );
        if ( Cpl::Math::areFloatsEqual( setpointDelta, 0.0 ) == false )
        {
            setpointChanged = true;
        }
    }

    // Update my cached active setpoint
    m_prevActiveSetpoint = newActiveSetpoint;


    //--------------------------------------------------------------------------
    // Post-Algorithm processing
    //--------------------------------------------------------------------------

    // Set my outputs
    m_out.setpointChanged->write( setpointChanged );
    m_out.idtDeltaError->write( deltaIdtError );
    m_out.setpointDelta->write( setpointDelta );
    m_out.activeSetpoint->write( newActiveSetpoint );

    // If I get here -->everything worked!
    return true;
}




