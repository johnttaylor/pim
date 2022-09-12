/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Private_.h"
#include "Storm/Thermostat/ModelPoints.h"



#define INITIAL_PRIMARY_IDT     75.0F
#define INITIAL_SECONDARY_IDT   71.0F

void initializeModelPoints() noexcept
{
    mp_primaryRawIdt.write( INITIAL_PRIMARY_IDT );
    mp_secondaryRawIdt.write( INITIAL_SECONDARY_IDT );
    mp_activeIdt.setInvalid();
    mp_relayOutputs.setSafeAllOff();
    mp_idtAlarms.setAlarm( false, false, false );
    mp_noActiveConditioningAlarm.setAlarm( false, false );
    mp_userCfgModeAlarm.setAlarm( false, false );
    mp_systemForcedOffRefCnt.reset();
    mp_systemConfig.setInvalid();           // Algorithm will update this!
    mp_systemOn.write( false );
    mp_resetPiPulse.write( false );
    mp_operatingModeChanged.write( false );
    mp_deltaIdtError.write( 0.0F );
    mp_deltaSetpoint.write( 0.0F );
    mp_setpointChanged.write( false );
    mp_activeSetpoint.setInvalid();         // Algorithm will update this!    
    mp_freezePiRefCnt.reset();
    mp_inhibitfRefCnt.reset();
    mp_pvOut.write( 0.0F );
    mp_sumError.write( 0.0F );
    mp_pvInhibited.write( false );
    Storm::Type::VirtualOutputs_T zeroVOutputs = { 0, };
    mp_vOutputs.write( zeroVOutputs );
    Storm::Type::CycleInfo_T zeroCycleInfo;
    mp_cycleInfo.write( zeroCycleInfo );
    Storm::Type::EquipmentTimes_T zeroEquipmentBeginTimes;
    mp_equipmentBeginTimes.write( zeroEquipmentBeginTimes );
}

