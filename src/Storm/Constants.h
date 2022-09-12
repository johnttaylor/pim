#ifndef Storm_Constants_h_
#define Storm_Constants_h_
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
/** @file
    
    This file provides magic constants that NOT component specific, i.e. general
    system constants
 */


#include "colony_config.h"

/** The maximum number of compressor heating stages that the system supports.
 */
#ifndef OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES
#define OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES      0
#endif

 /** The maximum number of cooling stages that the system supports.
  */
#ifndef OPTION_STORM_MAX_COMPRESSOR_COOLING_STAGES
#define OPTION_STORM_MAX_COMPRESSOR_COOLING_STAGES      1
#endif

/** The maximum number of compressor stages - independent of cooling/heating
 */
#define STORM_MAX_OUTDOOR_STAGES                        (OPTION_STORM_MAX_COMPRESSOR_COOLING_STAGES > OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES? OPTION_STORM_MAX_COMPRESSOR_COOLING_STAGES: OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES) 


/** The maximum number of indoor Electric heating stages that the system supports.
 */
#ifndef OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES
#define OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES        0
#endif

/** The maximum number of indoor furnace heating stages that the system supports.
 */
#ifndef OPTION_STORM_MAX_FURNACE_HEATING_STAGES
#define OPTION_STORM_MAX_FURNACE_HEATING_STAGES         3
#endif

/** The maximum number of indoor stages - independent of heating type
    (i.e max(OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES, OPTION_STORM_MAX_FURNACE_HEATING_STAGES)
 */
#define STORM_MAX_INDOOR_STAGES                         (OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES > OPTION_STORM_MAX_FURNACE_HEATING_STAGES? OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES: OPTION_STORM_MAX_FURNACE_HEATING_STAGES) 

/** The maximum number of heating stages that the system supports.  This is
    typically the combination of the number of compressor heating stages and
    indoor heat stages
 */
#ifndef OPTION_STORM_MAX_HEATING_STAGES
#define OPTION_STORM_MAX_HEATING_STAGES                 (OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES+STORM_MAX_INDOOR_STAGES)
#endif

 /** Maximum possible stage configuration - is defined as outdoor stages + indoor stages
  */
#ifndef OPTION_STORM_MAX_TOTAL_STAGES
#define OPTION_STORM_MAX_TOTAL_STAGES                   (STORM_MAX_OUTDOOR_STAGES + STORM_MAX_INDOOR_STAGES)
#endif

/** Default setting for Enabling Remote temperature sensor
 */
#ifndef OPTION_STORM_DEFAULT_REMOTE_SENSOR_ENABLED_STATE
#define OPTION_STORM_DEFAULT_REMOTE_SENSOR_ENABLED_STATE    false
#endif

/** The minimum compressor off time in seconds.
 */
#ifndef OPTION_STORM_MIN_COMPRESSOR_OFF_TIME_SEC
#define OPTION_STORM_MIN_COMPRESSOR_OFF_TIME_SEC        (5*60)
#endif

/** The minimum Indoor Heating off time in seconds.  Applies to all stages
    of indoor heat.
 */
#ifndef OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC
#define OPTION_STORM_MIN_INDOOR_HEATER_OFF_TIME_SEC     (3*60)
#endif

/** Default CPH setting
 */
#ifndef OPTION_STORM_DEFAULT_CPH
#define OPTION_STORM_DEFAULT_CPH                        Storm::Type::Cph::e3CPH
#endif

/** Default Minimum On cycle time (in seconds)
 */
#ifndef OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME
#define OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME          (5*60)
#endif

/** Default Minimum Off cycle time (in seconds)
 */
#ifndef OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME
#define OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME          (5*60)
#endif

/** Magic constant the represent the 'amount' LV per cooling stage
 */
#ifndef OPTION_STORM_COOLING_LV_PER_STAGE
#define OPTION_STORM_COOLING_LV_PER_STAGE               100.0F
#endif


/** Magic constant the represent the 'amount' LV per heating stage
 */
#ifndef OPTION_STORM_HEATING_LV_PER_STAGE
#define OPTION_STORM_HEATING_LV_PER_STAGE               100.0F
#endif

/** Cooling Indoor Fan speed (as percentage) when a system has variable speed
    Blower
 */
#ifndef OPTION_STORM_DEFAULT_VSP_BLOWER_COOLING_SPEED
#define OPTION_STORM_DEFAULT_VSP_BLOWER_COOLING_SPEED   (0.85)
#endif

/** Indoor Fan continuous speed (as percentage) when a system has variable speed
 */
#ifndef OPTION_STORM_DEFAULT_VSP_BLOWER_FAN_CONT_SPEED
#define OPTION_STORM_DEFAULT_VSP_BLOWER_FAN_CONT_SPEED  (0.5)
#endif

/** This symbol defines the nominal PI Gain constant for cooling
 */
#ifndef OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_GAIN
#define OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_GAIN   15.0F
#endif

/** This symbol defines the nominal PI reset constant for cooling.  The
    time units are milliseconds.
 */
#ifndef OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_RESET
#define OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_RESET  ((5*60*1000))   // 5 min reset time
#endif

/** This symbol defines the nominal PI Gain constant for heating
 */
#ifndef OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN
#define OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN   20.0F
#endif

/** This symbol defines the nominal PI reset constant for heating. The
    time units are milliseconds.
 */
#ifndef OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_RESET
#define OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_RESET  ((5*60*1000))   // 5 min reset time
#endif

/** This symbol defines the 'fast' PI Gain constant for cooling
 */
#ifndef OPTION_STORM_PI_CONSTANTS_COOLING_FAST_GAIN
#define OPTION_STORM_PI_CONSTANTS_COOLING_FAST_GAIN     20.0F
#endif

/** This symbol defines the 'fast' PI reset constant for cooling. The
    time units are milliseconds.
 */
#ifndef OPTION_STORM_PI_CONSTANTS_COOLING_FAST_RESET
#define OPTION_STORM_PI_CONSTANTS_COOLING_FAST_RESET    ((5*60*1000))   // 5 min reset time
#endif

/** This symbol defines the 'fast' PI Gain constant for heating
 */
#ifndef OPTION_STORM_PI_CONSTANTS_HEATING_FAST_GAIN
#define OPTION_STORM_PI_CONSTANTS_HEATING_FAST_GAIN     30.0F
#endif

/** This symbol defines the 'fast' PI reset constant for heating. The
    time units are milliseconds.
 */
#ifndef OPTION_STORM_PI_CONSTANTS_HEATING_FAST_RESET
#define OPTION_STORM_PI_CONSTANTS_HEATING_FAST_RESET    ((5*60*1000))   // 5 min reset time
#endif

/** This symbol defines the default maximum Process Variable's output
    value.
 */
#ifndef OPTION_STORM_PI_CONSTANTS_MAX_PV_OUT
#define OPTION_STORM_PI_CONSTANTS_MAX_PV_OUT            100
#endif

#endif  // end header latch
