#ifndef Storm_Thermostat_Ui_PicoDisplay_Api_h
#define Storm_Thermostat_Ui_PicoDisplay_Api_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file defines the top level entry points to initialize, execute, and
    shutdown the UI running on the Pico Display
 */

#include "Cpl/System/ElapsedTime.h"


 /// 
namespace Storm {
///
namespace Thermostat {
///
namespace Ui {
///
namespace PicoDisplay {

/** This method is used to initialize the UI logic.
    The application is required to call this method in the Periodic Scheduler's
    'beginThreading' call back function.
 */
void intializeUI();

/** The application is required to call this method from its
    periodic scheduler. The frequency of the interval determines
    the responsiveness of the UI.  Recommendation is to
    call this method at 10Hz
 */
void processUI( Cpl::System::ElapsedTime::Precision_T currentTick,
                Cpl::System::ElapsedTime::Precision_T currentInterval );

/** This method is used to cleanly shutdown the UI logic. The application is
    required to call this method in the Periodic Scheduler's 'endThreading'
    call back function.
 */
void shutdownUI();



/*-------------------------------------------------------*/
}   // end namespace(s)
}
}
}

#endif // end header latch

