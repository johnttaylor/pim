#ifndef Storm_Thermostat_Outputs_h_
#define Storm_Thermostat_Outputs_h_
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



///
namespace Storm {
///
namespace Thermostat {


/** This abstract/concrete interface defines the interface for updating the
    physical HVAC outputs.  It is called at the end of each Algorithm processing 
    cycle.
 */
class Outputs
{
public:
    /// Hook to updated the HVAC Outputs. What this mean is platform specific
    static void updateHVACOutputs();
};


};      // end namespace
};      // end namespace
#endif  // end header latch
