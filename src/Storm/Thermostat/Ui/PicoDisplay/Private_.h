
#ifndef Storm_Thermostat_Ui_PicoDisplay_Private_h
#define Storm_Thermostat_Ui_PicoDisplay_Private_h
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

    This file defines the INTERNAL interface used by the UI sub-system.  The
    Application should NEVER call/access these interfaces
 */

#include "Storm/Type/ThermostatMode.h"
#include "Storm/Type/FanMode.h"

/// 
namespace Storm {
///
namespace Thermostat {
///
namespace Ui {
///
namespace PicoDisplay {

/*-------------------------------------------------------*/
/// Updates the screen with the current 'working' Mode value
void drawMode( bool selectionActive = false );

/// Updates the screen with the current 'working' Fan value
void drawFan( bool selectionActive = false );

/// Updates the screen with the current indoor temperature value
void drawIdt();

/// Updates the screen with the current outdoor temperature value
void drawOdt();

/// Updates the screen with the current cooling setpoint value
void drawCoolingSetpoint( bool selectionActive = false );

/// Updates the screen with the current heating setpoint value
void drawHeatingSetpoint( bool selectionActive = false );

/// Updates the screen with the current outputs state
void drawHVACOutputs();


/*-------------------------------------------------------*/
/* Expose sub-system variables */

/// Current/working User mode. Is set to eOFF when no valid value
extern Storm::Type::ThermostatMode  g_uiMode;

/// Current/working Fan mode. Is set to eAUTO when no valid value
extern Storm::Type::FanMode         g_uiFan;

/// Current/working cooling setpoint. Is set to 0.0 when no valid value
extern float                        g_uiCoolingSetpoint;

/// Current/working heating setpoint. Is set to 0.0 when no valid value
extern float                        g_uiHeatingSetpoint;

}   // end namespace(s)
}
}
}

#endif // end header latch

