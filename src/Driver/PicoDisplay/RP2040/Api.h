#ifndef Driver_PicoDisplay_RP2040_API_h
#define Driver_PicoDisplay_RP2040_API_h
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

    This file defines the interface for initializing the concrete implementation
    of the PicoDisplay driver using the actual hardware

 */

#include "colony_config.h"
#include "pimoroni_common.hpp"
#include "pimoroni_bus.hpp"


/// Pin for Button A
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_A_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_A_PIN      12
#endif

/// Pin for Button B
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_B_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_B_PIN      13
#endif

/// Pin for Button X
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_X_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_X_PIN      14
#endif

/// Pin for Button Y
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_Y_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_Y_PIN      15
#endif

/// RGB LED: The pin for the RED PWM signal 
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_RED_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_RED_PIN       6
#endif

/// RGB LED: The pin for the GREEN PWM signal 
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_GREEN_PIN  
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_GREEN_PIN     7
#endif

/// RGB LED: The pin for the BLUE PWM signal 
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_BLUE_PIN  
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_BLUE_PIN      8
#endif

/// The default screen orientation
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_ROTATION         
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_ROTATION       pimoroni::ROTATE_0
#endif

/// The LCD Controller pins
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_PINS             
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_PINS           get_spi_pins( pimoroni::BG_SPI_FRONT )
#endif

///
namespace Driver {
///
namespace PicoDisplay {
///
namespace RP2040 {

/** This method is used to initialize the 'simulator'
 */
void initialize();


} // End namespace(s)
}
}

#endif // end header latch
