#ifndef Storm_Type_VirtualOutputs_h_
#define Storm_Type_VirtualOutputs_h_
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

#include "Storm/Constants.h"
#include <stdint.h>



///
namespace Storm {
///
namespace Type {

/** The following typedef is used to hold the virtual outputs for System. The 
    virtual outputs are later mapped to relay/discrete outputs and/or digital 
    communications to control the physical equipment.
 */
typedef struct
{
 
    uint16_t  indoorFan;                                        //!< Indoor Fan on/off/speed setting.  Range: 0=off, 1000=Full speed
    uint16_t  indoorFanCont;                                    //!< Indoor Fan on/off/speed setting for FAN CONTINOUS operation.  Range: 0=off, 1000=Full Speed
    uint16_t  indoorStages[STORM_MAX_INDOOR_STAGES];            //!< Output values per stage for the Indoor Unit.  Range: 0=off, 1000=Full capacity. The array is zero based so: index 0 == "stage 1"
    uint16_t  outdoorFan;                                       //!< Outdoor Fan on/off/speed setting.  Range: 0=off, 1000=Full speed
    uint16_t  outdoorStages[STORM_MAX_OUTDOOR_STAGES];          //!< Output values per stage for the Outdoor Unit.  Range: 0=off, 1000=Full capacity. The array is zero based so: index 0 == "stage 1"
    bool      sovInHeating;                                     //!< When set to true the Switch-Over-Value is set for heating operation; else it is set to cooling operation
} VirtualOutputs_T;


};      // end namespaces
};
#endif  // end header latch
