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


#include "FanControl.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"

#define SECT_   "Storm::Component::FanControl"


/// Namespaces
using namespace Storm::Component;



///////////////////////////////
FanControl::FanControl( struct Input_T ins, struct Output_T outs )
    : m_in( ins )
    , m_out( outs )
{
    CPL_SYSTEM_ASSERT( m_in.equipmentBeginTimes );
    CPL_SYSTEM_ASSERT( m_in.fanMode );
    CPL_SYSTEM_ASSERT( m_in.systemConfig );
    CPL_SYSTEM_ASSERT( m_in.vOutputs );
    CPL_SYSTEM_ASSERT( m_out.vOutputs );
}

bool FanControl::start( Cpl::System::ElapsedTime::Precision_T intervalTime )
{
    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool FanControl::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                          Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing
    //--------------------------------------------------------------------------

    // Get my inputs
    Storm::Type::FanMode            fanMode        = Storm::Type::FanMode::eAUTO;
    Storm::Type::SystemConfig_T     sysCfg         = { 0, };
    Storm::Type::VirtualOutputs_T   outputs        = { 0, };
    Storm::Type::EquipmentTimes_T   equipTimes     = { 0, };
    int8_t                          validMode      = m_in.fanMode->read( fanMode );
    int8_t                          validSystem    = m_in.systemConfig->read( sysCfg );
    int8_t                          validOutputs   = m_in.vOutputs->read( outputs );
    int8_t                          validEquipment = m_in.equipmentBeginTimes->read( equipTimes );
    if ( Cpl::Dm::ModelPoint::IS_VALID( validMode ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validSystem ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validOutputs ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validEquipment ) == false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "FanControl::execute. One or more invalid MPs (FanMode=%d, system=%d, vOutputs=%d equipTimes=%d", validMode, validSystem, validOutputs, validEquipment ) );

        // Force the fan off
        m_out.vOutputs->setIndoorFanOutput( STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
        return true;
    }


    //--------------------------------------------------------------------------
    // Algorithm processing
    //--------------------------------------------------------------------------

    // Simple Fan Continuous - Force the Fan ON
    if ( fanMode == +Storm::Type::FanMode::eCONTINUOUS )
    {
        m_out.vOutputs->setIndoorFanContinousOutput( sysCfg.fanContinuousSpeed );
    }
    else if ( fanMode == +Storm::Type::FanMode::eAUTO )
    {
        m_out.vOutputs->setIndoorFanContinousOutput( STORM_DM_MP_VIRTUAL_OUTPUTS_OFF );
    }

    //--------------------------------------------------------------------------
    // Post-Algorithm processing
    //--------------------------------------------------------------------------



    // If I get here -->everything worked!
    return true;
}


