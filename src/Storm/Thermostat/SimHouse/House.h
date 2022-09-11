#ifndef Storm_Thermostat_SimHouse_House_h_
#define Storm_Thermostat_SimHouse_House_h_
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

#include "colony_config.h"
#include "Storm/Utils/SimHouse.h"
#include "Cpl/System/EventLoop.h"

/** The periodic timing, in milliseconds, to run the house simulator
 */
#ifndef OPTION_STORM_THERMOSTAT_SIM_HOUSE_SIM_TIMING_MS
#define OPTION_STORM_THERMOSTAT_SIM_HOUSE_SIM_TIMING_MS        1000
#endif

 ///
namespace Storm {
///
namespace Thermostat {
///
namespace SimHouse {


/** This class glues the House Simulation into the Thermostat Test Application.
    Updates the mp_primaryRawIdt ModelPoint with the output of simulation - when
    the mp_houseSimEnabled MP is true.  The current outdoor temperature use
    by the simulation is set/access by the mp_outdoorTemp MP.
 */
class House : public Cpl::System::EventLoop
{
public:
    /// Constructor
    House();

protected:
    /// Override base implementation to access the event-loop timing directly
    void appRun();

    /// Helper method
    void executeSimulation();


protected:
    /// The underlying house simulation
    Storm::Utils::SimHouse  m_sim;
};

};      // end namespace(s)
};
};

#endif  // end header latch
