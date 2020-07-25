#ifndef Storm_Type_ThermostatMode_h_
#define Storm_Type_ThermostatMode_h_
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

/** The following enum/class defines the allowed Thermostat Modes.  The thermostat
    mode is set/selected by the User.

    @param ThermostatMode           - Class name for the Thermostat Mode enum
    @param int                      - storage type of the enum
    @param eOFF                     - The Thermostat is off
    @param eCOOLING                 - The Thermostat only operates in the cooling mode
    @param eHEATING                 - The Thermostat only operates in the heating mode
    @param eAUTO                    - The Thermostat will operate either cooling or heating mode depending on the demand
    @param eID_HEATING              - The Thermostat only operates in the heating mode AND ONLY using indoor heat (a.k.a 'Emergency Heat' mode when the system is configured with a HeatPump)
 */
BETTER_ENUM( ThermostatMode, int, eOFF, eCOOLING, eHEATING, eAUTO, eID_HEATING );


};      // end namespaces
};
#endif  // end header latch
