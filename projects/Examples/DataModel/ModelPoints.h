#ifndef ModelPoints_h_
#define ModelPoints_h_
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

This file defines all of the Model Points for the Thermostat Application.  
          
Note: the Model Points (and Database) are purposely placed in the 
      standard/global namespace to simplify their usage (i.e. less 
      verbose naming).
 */

#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "MpMetrics.h"
#include "MpAlarm.h"


/** Encapsulate all Model Points in the 'mp' namespace to prevent polluting
    the global name space
 */
namespace mp {

/// Input Signal.  Range is 1 to 1000
extern Cpl::Dm::Mp::Uint32      signalIn;

/** High Alarm.  When the alarm is asserted the model point value is valid.  
    When the alarm is de-asserted the model point is invalid.  
 */
extern MpAlarm                  hiAlarm;

/// Low Alarm.  Has the same semantics as the High Alarm
extern MpAlarm                  loAlarm;

/** The application's boot counter, aka the number of times the application 
    has been run.
 */
extern Cpl::Dm::Mp::Uint32      bootCounter;

/** The metrics collected by the 'Algorithm
 */
extern MpMetrics                metrics;

/// The Application's Model Point Database
extern Cpl::Dm::ModelDatabase   g_modelDatabase;

}       // end namespace
#endif  // end header latch
