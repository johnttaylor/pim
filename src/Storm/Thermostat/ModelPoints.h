#ifndef Storm_Thermostat_ModelPoints_h_
#define Storm_Thermostat_ModelPoints_h_
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
/** @file 

This file defines all of the Model Points for the Thermostat Application.  
          
Note: the Model Points (and Database) are purposely placed in the 
      standard/global namespace to simplify their usage (i.e. less 
      verbose naming).
 */

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



/// User Input: The current heating and cooling setpoints
extern Storm::Dm::MpSetpoints           mp_setpoints;

/// User Input: The currently selected operating mode (Off, cooling, heating, auto, etc.)
extern Storm::Dm::MpThermostatMode      mp_userMode;

/// User Input: The user selected Fan operating mode
extern Storm::Dm::MpFanMode             mp_fanMode;

/// User Input: The number of operation hours between Indoor Air filter changes
extern Cpl::Dm::Mp::Uint32              mp_maxAirFilterHours;


/// Sensor: Primary (aka on-board) indoor temperature
extern Cpl::Dm::Mp::Float               mp_primaryRawIdt;

/// Sensor: Secondary (aka remote) indoor temperature
extern Cpl::Dm::Mp::Float               mp_secondaryRawIdt;

/// Sensor: Active/Displayed indoor temperature sensor
extern Cpl::Dm::Mp::Float               mp_activeIdt;

/// Sensor: Current Outdoor temperature (not current requirement for ODT sensor, but it is used for/by the House simulator)
extern Cpl::Dm::Mp::Float               mp_outdoorTemp;


/// Output: The current HVAC relay/signal output values
extern Storm::Dm::MpHvacRelayOutputs    mp_relayOutputs;


/// Alarm: No Indoor temperature sensor/input available/working
extern Storm::Dm::MpIdtAlarm            mp_idtAlarms;

/// Alarm: Current configuration is such that no active condition can be provided
extern Storm::Dm::MpSimpleAlarm         mp_noActiveConditioningAlarm;

/// Alarm: The user has selected an mode of operation that is not supported by the current equipment settings
extern Storm::Dm::MpSimpleAlarm         mp_userCfgModeAlarm;

/// Alert: The Air Filter needs to be changed
extern Storm::Dm::MpSimpleAlarm         mp_airFilterAlert;


/// Configuration: Secondary IDT sensor enabled
extern Cpl::Dm::Mp::Bool                mp_enabledSecondaryIdt;

/// Configuration: HVAC Equipment settings
extern Storm::Dm::MpEquipmentConfig     mp_equipmentConfig;

/// Configuration: Comfort/Control-algorithm configuration settings
extern Storm::Dm::MpComfortConfig       mp_comfortConfig;


/// Internal: Tracks error/exception that force the system to be off
extern Cpl::Dm::Mp::RefCounter              mp_systemForcedOffRefCnt;

/// Internal: System Configuration attributes, parameters, settings, etc.
extern Storm::Dm::MpSystemConfig            mp_systemConfig;

/// Internal: The system is actively condition the space (not same as any equipment on)
extern Cpl::Dm::Mp::Bool                    mp_systemOn;

/// Internal: Tracks the start time for when the equipment is turned on/off
extern Storm::Dm::MpEquipmentBeginTimes     mp_equipmentBeginTimes;

/// Internal: Control Algorithm request to reset the PI component
extern Cpl::Dm::Mp::Bool                    mp_resetPiPulse;

/// Internal: Tracks when the operating mode has changed
extern Cpl::Dm::Mp::Bool                    mp_operatingModeChanged;

/// Internal: The current error between the IDT and the active setpoint
extern Cpl::Dm::Mp::Float                   mp_deltaIdtError;

/// Internal: The change in the active setpoint during the current processing cycle
extern Cpl::Dm::Mp::Float                   mp_deltaSetpoint;

/// Internal: Tracks when the active setpoint is changed during the current processing cycle
extern Cpl::Dm::Mp::Bool                    mp_setpointChanged;

/// Internal: The active setpoint (based on operating mode)
extern Cpl::Dm::Mp::Float                   mp_activeSetpoint;

/// Internal: Control Algorithm request to freeze the PI values/outputs
extern Cpl::Dm::Mp::RefCounter              mp_freezePiRefCnt;

/// Internal: Control Algorithm request to freeze/inhibit changes to the PI integral term
extern Cpl::Dm::Mp::RefCounter              mp_inhibitfRefCnt;

/// Internal The Process Variable output of the PI component (i.e. unit-less number that represent the load on the house)
extern Cpl::Dm::Mp::Float                   mp_pvOut;

/// Internal: The PI's component integral term
extern Cpl::Dm::Mp::Float                   mp_sumError;

/// Internal: Status output of the PI component to indicate that the PI component's output has been inhibited/frozen.
extern Cpl::Dm::Mp::Bool                    mp_pvInhibited;

/// Internal: Virtual HVAC outputs (these are later map to physical outputs)
extern Storm::Dm::MpVirtualOutputs          mp_vOutputs;

/// Internal: Tracks the current on/off cycling information
extern Storm::Dm::MpCycleInfo               mp_cycleInfo;

/// Internal: Free running counter the is incremented after every 'Algorithm processing cycle'
extern Cpl::Dm::Mp::Uint32                  mp_loopCounter;

/// Internal: The elapsed time that the indoor blower has been on.  This MP is reserved for use by the AirFilterMonitor Component, i.e. it is NOT a general 'elasped time' of how long the blower has been on
extern Cpl::Dm::Mp::ElapsedPrecisionTime    mp_airFilterOperationTime;


/// White Box Testing: When true, the minimum equipment off time is NOT enforced
extern Storm::Dm::MpWhiteBox                mp_whiteBox;


/// The Thermostat's Model Point Database
extern Cpl::Dm::ModelDatabase           g_modelDatabase;

#endif  // end header latch
