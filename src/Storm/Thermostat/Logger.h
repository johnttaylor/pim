#ifndef Storm_Thermostat_Logger_h_
#define Storm_Thermostat_Logger_h_
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

#include "Cpl/System/ElapsedTime.h"

///
namespace Storm {
///
namespace Thermostat {


/** This abstract/concrete interface defines the interface for a 'hook' to allow
    logging to occur at the end of the Algorithm processing cycle.
 */
class Logger
{
public:
    /// Hook to 'Log' the current system state.  What this mean is platform specific
    static void recordSystemData( Cpl::System::ElapsedTime::Precision_T currentInterval );
};


};      // end namespace
};      // end namespace
#endif  // end header latch
