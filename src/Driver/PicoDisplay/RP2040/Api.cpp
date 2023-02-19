/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Api.h"
#include "drivers/st7789/st7789.hpp"
#include "pico/stdlib.h"
#include "Driver/PicoDisplay/Api.h"
#include "Driver/LED/PimoroniPico/RedGreenBlue.h"
#include "Driver/Button/RP2040/Hal.h"



static Driver::Button::PolledDebounced buttonA_( { OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_A_PIN, true } );
static Driver::Button::PolledDebounced buttonB_( { OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_B_PIN, true } );
static Driver::Button::PolledDebounced buttonX_( { OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_X_PIN, true } );
static Driver::Button::PolledDebounced buttonY_( { OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_Y_PIN, true } );


static Driver::LED::PimoroniPico::RedGreenBlue rgbLEDDriver_( OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_RED_PIN, 
                                                              OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_GREEN_PIN, 
                                                              OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_BLUE_PIN );

pimoroni::ST7789 st7789_( OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH, 
                          OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT, 
                          OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_ROTATION, 
                          false, 
                          OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_PINS );


/////////////////////////////////////////////
void Driver::PicoDisplay::RP2040::initialize()
{
    driverButtonHalRP2040_initialize( buttonA_.getHandle() );
    driverButtonHalRP2040_initialize( buttonB_.getHandle() );
    driverButtonHalRP2040_initialize( buttonX_.getHandle() );
    driverButtonHalRP2040_initialize( buttonY_.getHandle() );
}

Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonA() { return buttonA_; }
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonB() { return buttonB_; }
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonX() { return buttonX_; }
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonY() { return buttonY_; }

Driver::LED::RedGreenBlue& Driver::PicoDisplay::Api::rgbLED() { return rgbLEDDriver_; }

void Driver::PicoDisplay::Api::updateLCD( pimoroni::PicoGraphics& graphics )
{
    st7789_.update( &graphics );
}
void Driver::PicoDisplay::Api::setLCDBrightness( uint8_t brightnessLevel )
{
    st7789_.set_backlight( brightnessLevel );
}

void Driver::PicoDisplay::Api::nop()
{
    // Do NOTHING!
}
