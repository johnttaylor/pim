#ifndef Storm_Type_SystemConfig_h_
#define Storm_Type_SystemConfig_h_
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
/** @file */


#include "Storm/Type/OperatingMode.h"
#include "Storm/Type/Cph.h"
#include "Storm/Type/IduType.h"
#include "Storm/Type/OduType.h"
#include "Storm/Constants.h"

///
namespace Storm {
///
namespace Type {

/** The following typedef information per stage
 */
typedef struct
{
    float       lowerBound;         //!< Inclusive lower bound for the PV variable (aka load)
    float       upperBound;         //!< Inclusive upper bound for the PV variable (aka load)
    uint16_t    minIndoorFan;       //!< Minimum indoor fan speed associated with the stage (Range: 0=off, 1000=Full speed)
    uint16_t    maxIndoorFan;       //!< Maximum indoor fan speed associated with the stage (Range: 0=off, 1000=Full speed)
    uint32_t    minOnTime;          //!< Minimum on time in seconds
    uint32_t    minOffTime;         //!< Minimum off time in seconds
    int         cph;                //!< Cycle-Per-Hour settings.   The actual type is: Storm::Type::Cph ('betterenums' do not play well with classes/struct)
} Stage_T;

/** The following typedef contains system attributes, parameters, bounds, etc. 
    as derived from the current equipment configuration for CURRENT operation
    mode.
 */
typedef struct
{
    Stage_T     stages[OPTION_STORM_MAX_TOTAL_STAGES];      //!< Configuration/Information by stage.  Zero indexed by 'system stage' with compressor stages (if any) are first, followed by indoor stages (if any). For example in Heating operation: 2 stage compressor with 2 stage furnace would be: [0]=1st compressor stage, [1]=2nd compressor stage, [3]=1st furnace stage, [4]=2nd furnace stage 
    int         currentOpMode;                              //!< Current operating mode. The actual type is: Storm::Type::OperatingMode
    int         outdoorUnitType;                            //!< Outdoor Unit type. The actual type is: Storm::Type::OduType
    int         indoorUnitType;                             //!< Indoor Unit Type. The actual type is: Storm::Type::IduType
    uint16_t    numCompressorStages;                        //!< Number of active/configured compressor stages
    uint16_t    numIndoorStages;                            //!< Number of  active/configured indoor stages
    uint16_t    totalStages;                                //!< Total number of active stages (i.e. numCompressorStages + numIndoorStages)
    uint16_t    fanContinuousSpeed;                         //!< Indoor Fan Continuous speed setting.  (Range: 0=off, 1000=Full speed)
    float       gain;                                       //!< The proportional gain constant for the PI
    float       reset;                                      //!< The PI Reset time in milliseconds. The reset time is the time it takes for the integral term, given a constant error, to effect PI OUT term the same amount as the Proportional gain. For example a 10 minute reset time would be: (10 * 60 * 1000) 
    float       maxPvOut;                                   //!< The maximum allowed 'Process Value' value output of the PI
} SystemConfig_T;



};      // end namespaces
};
#endif  // end header latch
