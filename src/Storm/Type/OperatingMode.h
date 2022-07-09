#ifndef Storm_Type_OperatingMode_h_
#define Storm_Type_OperatingMode_h_
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


#include "Cpl/Type/enum.h"


///
namespace Storm {
///
namespace Type {

/** The following enum/class defines the Operating modes of the thermostat.  The
    operating mode represent the equipment/system mode of operation.

    @param OperatingMode            - Class name for the Operating Mode enum
    @param int                      - storage type of the enum
    @param eOFF                     - The system/equipment is off
    @param eCOOLING                 - The system/equipment only operates in the cooling mode
    @param eHEATING                 - The system/equipment only operates in the heating mode
    @param eID_HEATING              - The system/equipment only operates in the heating mode AND only uses Indoor Heating sources
 */
BETTER_ENUM( OperatingMode, int, eOFF, eCOOLING, eHEATING, eID_HEATING, eUNKNOWN );


};      // end namespaces
};
#endif  // end header latch
