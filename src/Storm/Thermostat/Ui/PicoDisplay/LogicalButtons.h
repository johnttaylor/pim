#ifndef Storm_Thermostat_Ui_PicoDisplay_LogicalButtons_h
#define Storm_Thermostat_Ui_PicoDisplay_LogicalButtons_h
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

    This file provide the logic for sampling the low level button drivers and
    generating logical buttons and button events (e.g repeat events for button 
    pressed for 'long time).

    NOTE: The initial 'pressed event' is generated on the physical RELEASE of
          the button OR the press-n-hold time criteria has been met.

    The button events for the logical buttons are reported via Model points.  The MPs
    will generate change notifications where their change state.  Note: A repeat-event
    will generate change notifications.

    The Model Points are:
        mp_buttonEventMode      // aka A
        mp_buttonEventFan       // aka B
        mp_buttonEventUp        // aka X
        mp_buttonEventDown      // aka Y
 */

#include "Cpl/System/ElapsedTime.h"


/** Specifies the initial hold time, in interval counts, before
    a pressed button generates a repeat event
 */
#ifndef OPTION_STORM_THERMOSTAT_UI_PICODISPLAY_LOGICAL_BUTTON_HOLD_COUNT
#define OPTION_STORM_THERMOSTAT_UI_PICODISPLAY_LOGICAL_BUTTON_HOLD_COUNT     50  // 00.5 sec @100Hz
#endif

/** Specifies the repeat hold time, in interval counts, between
    a pressed button repeat events
 */
#ifndef OPTION_STORM_THERMOSTAT_UI_PICODISPLAY_LOGICAL_BUTTON_REPEAT_COUNT
#define OPTION_STORM_THERMOSTAT_UI_PICODISPLAY_LOGICAL_BUTTON_REPEAT_COUNT   10  // 0.1 sec @100Hz
#endif

/// 
namespace Storm {
///
namespace Thermostat {
///
namespace Ui {
///
namespace PicoDisplay {

/*-------------------------------------------------------*/
/** This method is used to initialize the Logical Button
    logic.  The application is required to call this method
    in the Periodic Scheduler's 'beginThreading' call back
    function.
 */
void intializeLogicalButtons();

/** The application is required to call this method from its
    periodic scheduler. The frequency of the interval determines
    the de-bounce AND the repeat times.  Recommendation is to
    call this method at 100Hz
 */
void processLogicalButtons( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval );


/** This method is used to cleanly shutdown the Logical Button
    logic.  The application is required to call this method
    in the Periodic Scheduler's 'endThreading' call back
    function.
 */
void shutdownLogicalButtons();


/*-------------------------------------------------------*/
}   // end namespace(s)
}
}
}

#endif // end header latch
