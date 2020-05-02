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


#include "IdtSelection.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"

#define SECT_               "Storm::Component::IdtSelection"


/// Namespaces
using namespace Storm::Component;


///////////////////////////////
IdtSelection::IdtSelection( struct Input_T inputs, struct Output_T outputs )
    : m_in( inputs )
    , m_out( outputs )
    , m_critical( false )
{
    CPL_SYSTEM_ASSERT( m_in.enabledSecondaryIdt );
    CPL_SYSTEM_ASSERT( m_in.primaryIdt );
    CPL_SYSTEM_ASSERT( m_in.secondaryIdt );
    CPL_SYSTEM_ASSERT( m_out.activeIdt );
    CPL_SYSTEM_ASSERT( m_out.idtAlarms );
    CPL_SYSTEM_ASSERT( m_out.systemForcedOffRefCnt );
}

bool IdtSelection::start( Cpl::System::ElapsedTime::Precision_T intervalTime )
{
    // Initialize my data
    m_critical = false;

    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool IdtSelection::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval
)
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing
    //--------------------------------------------------------------------------

    // Read my inputs
    bool   enabledSecondaryIdt   = false;
    float  primaryIdt            = 0.0F; // Default to 'bad value' 
    float  secondaryIdt          = 0.0F; // Default to 'bad value'
    int8_t validPrimary          = m_in.primaryIdt->read( primaryIdt );
    int8_t validSecondary        = m_in.secondaryIdt->read( secondaryIdt );
    int8_t validEnabledSecondary = m_in.enabledSecondaryIdt->read( enabledSecondaryIdt );
    if ( Cpl::Dm::ModelPoint::IS_VALID( validEnabledSecondary ) == false || enabledSecondaryIdt == false )
    {
        validSecondary = OPTION_CPL_DM_MODEL_POINT_STATE_INVALID;
    }

    //--------------------------------------------------------------------------
    // Algorithm processing
    //--------------------------------------------------------------------------

    // Select which IDT input to use
    float idt = primaryIdt;
    if ( enabledSecondaryIdt )
    {
        if ( Cpl::Dm::ModelPoint::IS_VALID( validSecondary ) )
        {
            idt = secondaryIdt;
        }
    }

    // When I am not configured for a secondary IDT -.set the 'valid' status to true (because it will be used as the alarm status, i.e. no alarm for secondary IDT when not configured)
    else
    {
        validSecondary = Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID;
    }

    // Determine the Alarm critical state
    bool critical = false;
    if ( ( Cpl::Dm::ModelPoint::IS_VALID( validPrimary ) == false && enabledSecondaryIdt == false ) ||
        ( Cpl::Dm::ModelPoint::IS_VALID( validPrimary ) == false && Cpl::Dm::ModelPoint::IS_VALID( validSecondary ) == false ) )
    {
        critical = true;
    }


    //--------------------------------------------------------------------------
    // Post-Algorithm processing
    //--------------------------------------------------------------------------

    // All done -.set the outputs
    m_out.activeIdt->write( idt );
    m_out.idtAlarms->setAlarm( !Cpl::Dm::ModelPoint::IS_VALID( validPrimary ), !Cpl::Dm::ModelPoint::IS_VALID( validSecondary ), critical );

    // Update the forced-off reference counter - BUT only on transitions.
    if ( critical == true && m_critical == false )
    {
        m_critical = true;
        m_out.systemForcedOffRefCnt->increment();
    }
    else if ( critical == false && m_critical == true )
    {
        m_critical = false;
        m_out.systemForcedOffRefCnt->decrement();
    }

    return true;
}




