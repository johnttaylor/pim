#ifndef Storm_Thermostat_SimHouse_ModelPoints_h_
#define Storm_Thermostat_SimHouse_ModelPoints_h_
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

This file defines test/simulation specific Model Points for the Thermostat 
Application.  
          
Note: the Model Points (and Database) are purposely placed in the 
      standard/global namespace to simplify their usage (i.e. less 
      verbose naming).
 */
#include "Cpl/Dm/Mp/Bool.h"



/// House Simulation: Used to enable/disable the house simulator
extern Cpl::Dm::Mp::Bool                mp_houseSimEnabled;

#endif  // end header latch
