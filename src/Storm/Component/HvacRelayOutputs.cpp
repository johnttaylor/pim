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


#include "HvacRelayOutputs.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"

#define SECT_   "Storm::Component::HvacRelayOutput"


/// Namespaces
using namespace Storm::Component;



///////////////////////////////
HvacRelayOutputs::HvacRelayOutputs( struct Input_T ins, struct Output_T outs )
    : m_in( ins )
    , m_out( outs )
    , m_systemOn( true )    // Initialize to true to trigger an off-edge the first time I run (This ASSUMES that mp_systemOn will be FALSE the first time the component executes)
    , m_y1( true )          // Same comment as above (assume mp_relayOutputs is all initialized to zeros)
    , m_w1( true )          // Same comment as above (assume mp_relayOutputs is all initialized to zeros)
{
    CPL_SYSTEM_ASSERT( m_in.equipmentBeginTimes );
    CPL_SYSTEM_ASSERT( m_in.systemForcedOffRefCnt );
    CPL_SYSTEM_ASSERT( m_in.systemOn );
    CPL_SYSTEM_ASSERT( m_in.vOutputs );
    CPL_SYSTEM_ASSERT( m_out.equipmentBeginTimes );
    CPL_SYSTEM_ASSERT( m_out.relayOutputs );
}

bool HvacRelayOutputs::start( Cpl::System::ElapsedTime::Precision_T intervalTime )
{
    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool HvacRelayOutputs::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                                Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing
    //--------------------------------------------------------------------------

    // Get my inputs
    bool                            failOff         = false;
    Storm::Type::VirtualOutputs_T   outputs         = { 0, };
    Storm::Type::EquipmentTimes_T   equipTimes      = { 0, };
    bool                            systemOn        = false;
    uint32_t                        forcedOffRefCnt = 0;
    int8_t                          validOutputs    = m_in.vOutputs->read( outputs );
    int8_t                          validEquipment  = m_in.equipmentBeginTimes->read( equipTimes );
    int8_t                          validForceCnt   = m_in.systemForcedOffRefCnt->read( forcedOffRefCnt );
    int8_t                          ValidSysOn      = m_in.systemOn->read( systemOn );
    if ( Cpl::Dm::ModelPoint::IS_VALID( validForceCnt ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validOutputs ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( ValidSysOn ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validEquipment ) == false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "HvacRelayOutputs::execute. One or more invalid MPs (forceCnd=%d, vOutputs=%d equipTimes=%d, sysOn=%d", validForceCnt, validOutputs, validEquipment, ValidSysOn ) );

        // Force all output to 'off'
        failOff = true;
    }


    //--------------------------------------------------------------------------
    // Algorithm processing
    //--------------------------------------------------------------------------

    Storm::Type::HvacRelayOutputs_T relaysNew = { 0, };

    // Force the system if there is 'hard' failure.  Note: In theory the system 
    // will have already been forced to the off mode at this point - but belts
    // and suspenders and all that...
    if ( failOff || forcedOffRefCnt > 0 )
    {
        Storm::Dm::MpVirtualOutputs::setSafeAllOff( outputs );
    }

    // Resolve the actual Fan speed
    uint16_t finalFanSpeed = outputs.indoorFan;
    if ( outputs.indoorFanCont > finalFanSpeed )
    {
        finalFanSpeed = outputs.indoorFanCont;
    }

    // Update the relays outputs based on the virtual output settings
    relaysNew.g  = finalFanSpeed > STORM_DM_MP_VIRTUAL_OUTPUTS_OFF ? true : false;
    relaysNew.bk = ( finalFanSpeed + 5 ) / 10;  // Convert to percentage as value between 0 and 100
    relaysNew.o  = !outputs.sovInHeating;
    relaysNew.w1 = outputs.indoorStages[0] > STORM_DM_MP_VIRTUAL_OUTPUTS_OFF ? true : false;
    relaysNew.w2 = outputs.indoorStages[1] > STORM_DM_MP_VIRTUAL_OUTPUTS_OFF ? true : false;
    relaysNew.w3 = outputs.indoorStages[2] > STORM_DM_MP_VIRTUAL_OUTPUTS_OFF ? true : false;
    relaysNew.y1 = outputs.outdoorStages[0] > STORM_DM_MP_VIRTUAL_OUTPUTS_OFF ? true : false;
    relaysNew.y2 = false; // outputs.outdoorStages[1] > STORM_DM_MP_VIRTUAL_OUTPUTS_OFF? true : false;

    // Detect outdoor unit begin on/off times
    if ( m_y1 == true && relaysNew.y1 == false )
    {
        equipTimes.outdoorUnitBeginOffTime = currentInterval;
    }
    else if ( m_y1 == false && relaysNew.y1 == true )
    {
        equipTimes.outdoorUnitBeginOnTime = currentInterval;
    }
    m_y1 = relaysNew.y1;

    // Detect indoor unit begin on/off times
    if ( m_w1 == true && relaysNew.w1 == false )
    {
        equipTimes.indoorUnitBeginOffTime = currentInterval;
    }
    else if ( m_w1 == false && relaysNew.w1 == true )
    {
        equipTimes.indoorUnitBeginOnTime = currentInterval;
    }
    m_w1 = relaysNew.w1;

    // Detect system on/off times
    if ( systemOn == false && m_systemOn == true )
    {
        equipTimes.systemBeginOffTime = currentInterval;
    }
    else if ( systemOn == true && m_systemOn == false )
    {
        equipTimes.systemBeginOnTime = currentInterval;
    }
    m_systemOn = systemOn;


    //--------------------------------------------------------------------------
    // Post-Algorithm processing
    //--------------------------------------------------------------------------

    // Update my outputs
    m_out.equipmentBeginTimes->write( equipTimes );
    m_out.relayOutputs->write( relaysNew );

    // If I get here -->everything worked!
    return true;
}


