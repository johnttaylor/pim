#ifndef Storm_Thermostat_Main_Pico_SimHouse_h_
#define Storm_Thermostat_Main_Pico_SimHouse_h_
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

#include "Storm/Thermostat/SimHouse/House.h"



///
namespace Storm {
///
namespace Thermostat {
///
namespace Main {


/** This concrete class is used to make the parent's executeSimulation()
    accessible to be called directly from the Periodic scheduler.

    The scheduleSimulation() method is required to be called at 1Hz frequency.
 */
class ScheduledSimHouse : public Storm::Thermostat::SimHouse::House
{
public:
    /// Constructor
    ScheduledSimHouse() : House(){};

public:
    /// Call the parent's execute method
    void scheduleSimulation() { executeSimulation(); }
};

};      // end namespace(s)
};
};

#endif  // end header latch
