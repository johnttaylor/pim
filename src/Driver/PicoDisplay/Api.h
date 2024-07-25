
#ifndef Driver_PicoDisplay_Api_h_
#define Driver_PicoDisplay_Api_h_
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
/** @file */


#include "colony_config.h"
#include "pico_graphics.hpp" 
#include "Driver/Button/PolledDebounced.h"
#include "Driver/LED/RedGreenBlue.h"

/// The width, in pixels, of the Display
#ifndef OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH
#define OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH          240
#endif

/// The height, in pixels, of the Display
#ifndef OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT
#define OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT         135
#endif

///
namespace Driver {
///
namespace PicoDisplay {

/** This class defines an interface for accessing the 'hardware bits' of the
    Pico Display board.

    The class is NOT thread safe.  The application is RESPONSIBLE for ensuring
    thread-safe usage.
 */
class Api
{
public:
    /// This method returns the Button driver for the board's 'A' button
    static Driver::Button::PolledDebounced& buttonA() noexcept;

    /// This method returns the Button driver for the board's 'B' button
    static Driver::Button::PolledDebounced& buttonB() noexcept;

    /// This method returns the Button driver for the board's 'X' button
    static Driver::Button::PolledDebounced& buttonX() noexcept;

    /// This method returns the Button driver for the board's 'Y' button
    static Driver::Button::PolledDebounced& buttonY() noexcept;

public:
    /// This method returns the RGB LED driver for the board's RGB LED
    static Driver::LED::RedGreenBlue& rgbLED() noexcept;

public:
    /** This method is used to update the display/LCD Controller with the
        specified frame buffer contents.
     */
    static void updateLCD( pimoroni::PicoGraphics& graphics );

    /// This method sets the back-light brightness of the LCD (range: 0 - 255)
    static void setLCDBrightness( uint8_t brightnessLevel );

    /** This method performs a 'null operation' on the display board.  What
        'null operation' means is target dependent (e.g. this method does
        nothing on the physical display, but on the Simulator it updates
        the simulation time on the GUI screen
    */
    static void nop();
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
