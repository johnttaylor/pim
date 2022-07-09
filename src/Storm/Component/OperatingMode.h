#ifndef Storm_Component_OperatingMode_h_
#define Storm_Component_OperatingMode_h_
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
/** @file */

#include "colony_config.h"
#include "Storm/Component/Base.h"
#include "Storm/Dm/MpSetpoints.h"
#include "Storm/Dm/MpThermostatMode.h"
#include "Storm/Dm/MpSimpleAlarm.h"
#include "Storm/Dm/MpEquipmentBeginTimes.h"
#include "Storm/Dm/MpSystemConfig.h"
#include "Storm/Dm/MpEquipmentConfig.h"
#include "Storm/Dm/MpComfortConfig.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/RefCounter.h"
#include "Cpl/Dm/Mp/ElapsedPrecisionTime.h"


/** This constant defines the negative cooling offset (in degrees 'F) for
    preferring cooling operating mode over heating mode.
 */
#ifndef OPTION_STORM_COMPONENT_OPERATING_MODE_COOLING_OFFSET
#define OPTION_STORM_COMPONENT_OPERATING_MODE_COOLING_OFFSET        1.0f
#endif   

 /** This constant defines the minimum separation (in degrees 'F) between the
     cooling and heating set-points when the User has selected 'Auto' mode.
   */
#ifndef OPTION_STORM_COMPONENT_OPERATING_MODE_MIN_SETPOINT_DELTA
#define OPTION_STORM_COMPONENT_OPERATING_MODE_MIN_SETPOINT_DELTA    4.0f
#endif   

   /** This constant defines the time hysteresis, in seconds, for how long the
       system must be off (no active heating/cooling) before allow a change in
       the Operating Mode when the User mode is in AUTO.  The default is
       10 minutes.
    */
#ifndef OPTION_STORM_COMPONENT_OPERATING_MODE_SECONDS_HYSTERESIS
#define OPTION_STORM_COMPONENT_OPERATING_MODE_SECONDS_HYSTERESIS    (10 * 60)
#endif   


    /// 
namespace Storm {
/// 
namespace Component {

/** This concrete class determine the actual mode of operation for the
    thermostat when the user has selected "Auto Mode".  It is also responsible
    for the following:

    1) Determine the 'active' set-point and calculation the delta error value
       for use by the PI Component.
    2) Populate the SystemConfig model point based on the actual operation mode.
 */
class OperatingMode : public Base
{
public:
    /// Input Model Points
    struct Input_T
    {
        Storm::Dm::MpSetpoints*             setpoints;              //!< Cooling & Heating set-points in degrees Fahrenheit
        Storm::Dm::MpThermostatMode*        userMode;               //!< The thermostat mode to be resolved
        Cpl::Dm::Mp::Float*                 idt;                    //!< The current indoor temperature in degrees Fahrenheit
        Storm::Dm::MpEquipmentBeginTimes*   equipmentBeginTimes;    //!< The begin times for when the HVAC outputs turned on/off
        Cpl::Dm::Mp::Bool*                  systemOn;               //!< Indicates that system is actively Cooling or Heating.  Note: this is not the same thing as the equipment is physically on, e.g I am actively conditioning the space - but currently in an off cycle
        Cpl::Dm::Mp::RefCounter*            systemForcedOffRefCnt;	//!< Reference Counter: When greater the zero the system is required to be forced off.
        Storm::Dm::MpEquipmentConfig*       equipmentConfig;        //!< The current Equipment configuration
        Storm::Dm::MpComfortConfig*         comfortConfig;          //!< The current comfort configuration settings
    }; 


    /// Output Parameters
    struct Output_T
    {
        Cpl::Dm::Mp::Bool*                  operatingModeChanged;       //!< When true, indicates that the operating mode changed during the processing; else the output is set to false
        Cpl::Dm::Mp::Bool*                  pulseResetPi;               //!< Triggers a reset-the-PI-controller request
        Cpl::Dm::Mp::RefCounter*            systemForcedOffRefCnt;	    //!< Reference Counter: When greater the zero the system is required to be forced off.
        Storm::Dm::MpSystemConfig*          systemConfig;               //!< Current system configuration based on equipment and current operating mode
        Storm::Dm::MpSimpleAlarm*           noActiveConditioningAlarm;  //!< Alarm MP used indicate that system configuration does NOT provide any active conditional (i.e. no heating and no cooling capacity)
        Storm::Dm::MpSimpleAlarm*           userConfigModeAlarm;        //!< Alarm MP used indicate that user mode is not compatible with the allowed modes operation for the system
    };

public:
    /// Constructor
    OperatingMode( struct Input_T ins, struct Output_T outs );

    /// Component specific initialization
    bool start( Cpl::System::ElapsedTime::Precision_T& newInterval );

protected:
    /// Inputs
    struct Input_T              m_in;

    /// Outputs
    struct Output_T             m_out;

    /// Current/Previous operating mode (because of limitations of BETTER_ENUM - when used inside a class - we use a simply integer to hold the enum value)
    int                         m_prevOperatingMode;

    /// Use to detected changes in the Equipment Config
    uint16_t                    m_equipCfgSequenceNumber;

    /// Use to detected changes in the Comfort Config
    uint16_t                    m_comfortCfgSequenceNumber;

    /// Flag used to detect the transition to AUTO mode
    bool                        m_inAuto;

    /// Flag that tracks when based on invalid system type that the system is being forced off
    bool                        m_forcedOff;


protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval );


protected:
    /// Helper method - determine allowed operating mode
    virtual void determineAllowedModes( const Storm::Dm::MpEquipmentConfig::Data& equipmentCfg, bool& haveCooling, bool& haveHeating, bool& haveHeatPump );

    /// Helper method - populates the system configuration based on the current operating mode and equipment configuration
    virtual void setSystemConfig( Storm::Type::OperatingMode newOpMode, Storm::Type::SystemConfig_T& sysCfg, const Storm::Dm::MpEquipmentConfig::Data& equipmentCfg, const Storm::Type::ComfortConfig_T& comfortCfg );

    /// Helper method
    virtual void setNewOperatingMode( Storm::Type::OperatingMode   newOpMode,
                                      bool                         haveHeatPump,
                                      const                        Storm::Dm::MpEquipmentConfig::Data& equipmentCfg,
                                      const                        Storm::Type::ComfortConfig_T& comfortCfg );

    /// Helper method
    virtual void setNoActiveConditioningAlarm( bool alarmIsActive ) noexcept;
};



};      // end namespace
};      // end namespace
#endif  // end header latch


