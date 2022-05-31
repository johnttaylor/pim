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

#include "Cpl/Dm/ModelDatabase.h"
#include "Storm/Dm/MpIdtAlarm.h"
#include "Storm/Dm/MpSimpleAlarm.h"
#include "Storm/Dm/MpSetpoints.h"
#include "Storm/Dm/MpFanMode.h"
#include "Storm/Dm/MpThermostatMode.h"
#include "Storm/Dm/MpEquipmentConfig.h"
#include "Storm/Dm/MpVirtualOutputs.h"
#include "Storm/Dm/MpEquipmentBeginTimes.h"
#include "Storm/Dm/MpComfortConfig.h"
#include "Storm/Dm/MpCycleInfo.h"
#include "Storm/Dm/MpSystemConfig.h"
#include "Storm/Dm/MpHvacRelayOutputs.h"
#include "Storm/Dm/MpWhiteBox.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/RefCounter.h"
#include "Cpl/Dm/Mp/ElapsedPrecisionTime.h"
#include <string.h>


// Help macros because I am OCD. The model point variable 
// name is prefixed with 'mp_'. For example:
// MP_INVALID(Cpl::Dm::Mp::Float, bob) == static Cpl::Dm::Mp::Float mp_bob;

// Creates model point in the invalid state. 
#define MP_INVALID(t, n)    static t mp_##n(modelDb_, #n ) 

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
MP_INVALID( Cpl::Dm::Mp::Float, primaryRawIdt );
MP_INVALID( Cpl::Dm::Mp::Float, secondaryRawIdt );
MP_INVALID( Cpl::Dm::Mp::Bool, enabledSecondaryIdt );
MP_INVALID( Cpl::Dm::Mp::Float, activeIdt );
MP_INVALID( Storm::Dm::MpIdtAlarm, idtAlarms );
MP_INVALID( Cpl::Dm::Mp::RefCounter, systemForcedOffRefCnt );

MP_INVALID( Storm::Dm::MpSimpleAlarm, noActiveConditioningAlarm );
MP_INVALID( Storm::Dm::MpSystemConfig, systemConfig );
MP_INVALID( Storm::Dm::MpEquipmentConfig, equipmentConfig );

MP_INVALID( Storm::Dm::MpSetpoints, setpoints );
MP_INVALID( Storm::Dm::MpThermostatMode, userMode );
MP_INVALID( Cpl::Dm::Mp::Bool, systemOn );
MP_INVALID( Storm::Dm::MpEquipmentBeginTimes,  equipmentBeginTimes );
MP_INVALID( Cpl::Dm::Mp::Bool, resetPiPulse );
MP_INVALID( Cpl::Dm::Mp::Bool, operatingModeChanged );
MP_INVALID( Storm::Dm::MpSimpleAlarm, userCfgModeAlarm );

MP_INVALID( Cpl::Dm::Mp::Float, deltaIdtError );
MP_INVALID( Cpl::Dm::Mp::Float, deltaSetpoint );
MP_INVALID( Cpl::Dm::Mp::Bool, setpointChanged );
MP_INVALID( Cpl::Dm::Mp::Float, activeSetpoint );

MP_INVALID( Cpl::Dm::Mp::RefCounter, freezePiRefCnt );
MP_INVALID( Cpl::Dm::Mp::RefCounter, inhibitfRefCnt );
MP_INVALID( Cpl::Dm::Mp::Float, pvOut );
MP_INVALID( Cpl::Dm::Mp::Float, sumError );
MP_INVALID( Cpl::Dm::Mp::Bool, pvInhibited );

MP_INVALID( Storm::Dm::MpVirtualOutputs, vOutputs ); 
MP_INVALID( Storm::Dm::MpCycleInfo, cycleInfo );
MP_INVALID( Storm::Dm::MpComfortConfig, comfortConfig );

MP_INVALID( Storm::Dm::MpFanMode, fanMode );

MP_INVALID( Storm::Dm::MpHvacRelayOutputs, relayOutputs ); 

MP_INVALID( Storm::Dm::MpWhiteBox, whiteBox );

MP_INVALID( Cpl::Dm::Mp::Uint32, maxAirFilterHours );
MP_INVALID( Storm::Dm::MpSimpleAlarm, airFilterAlert );
MP_INVALID( Cpl::Dm::Mp::ElapsedPrecisionTime, airFilterOperationTime );
