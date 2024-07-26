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
#include "Driver/PicoDisplay/Api.h"
#include "Driver/LED/Pimoroni/RedGreenBlue.h"
#include "Driver/Button/STM32/Hal.h"
#include "Driver/DIO/Out.h"
#include "Driver/DIO/Pwm.h"
#include "Driver/SPI/STM32/Master.h"
#include "Cpl/System/Api.h"



static DriverButtonPinHalSTM32_T     pinButtonA_( OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_A_PORT, OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_A_PIN, true );
static DriverButtonPinHalSTM32_T     pinButtonB_( OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_B_PORT, OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_B_PIN, true );
static DriverButtonPinHalSTM32_T     pinButtonX_( OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_X_PORT, OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_X_PIN, true );
static DriverButtonPinHalSTM32_T     pinButtonY_( OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_Y_PORT, OPTION_DRIVER_PICO_DISPLAY_STM32_BUTTON_Y_PIN, true );
static Driver::Button::PolledDebounced buttonA_( pinButtonA_ );
static Driver::Button::PolledDebounced buttonB_( pinButtonB_ );
static Driver::Button::PolledDebounced buttonX_( pinButtonX_ );
static Driver::Button::PolledDebounced buttonY_( pinButtonY_ );

static DriverDioPwmSTM32Config_T     ledRPwm_( OPTION_DRIVER_PICO_DISPLAY_STM32_RGB_RED_TIMER, OPTION_DRIVER_PICO_DISPLAY_STM32_RGB_RED_CHANNEL);
static DriverDioPwmSTM32Config_T     ledGPwm_( OPTION_DRIVER_PICO_DISPLAY_STM32_RGB_GREEN_TIMER, OPTION_DRIVER_PICO_DISPLAY_STM32_RGB_GREEN_CHANNEL );
static DriverDioPwmSTM32Config_T     ledBPwm_( OPTION_DRIVER_PICO_DISPLAY_STM32_RGB_BLUE_TIMER, OPTION_DRIVER_PICO_DISPLAY_STM32_RGB_BLUE_CHANNEL );
static Driver::DIO::Pwm              pwmLedRDriver_( ledRPwm_ );
static Driver::DIO::Pwm              pwmLedGDriver_( ledGPwm_ );
static Driver::DIO::Pwm              pwmLedBDriver_( ledBPwm_ );

static Driver::LED::Pimoroni::RedGreenBlue rgbLEDDriver_( pwmLedRDriver_,
                                                          pwmLedGDriver_,
                                                          pwmLedBDriver_ );

static DriverDioPwmSTM32Config_T     backlightPwm_( OPTION_DRIVER_PICO_DISPLAY_STM32_BACKLIGHT_PWM_TIMER, OPTION_DRIVER_PICO_DISPLAY_STM32_BACKLIGHT_PWM_CHANNEL);
static DriverDioOutSTM32PinConfig_T  chipSelectDriver_( OPTION_DRIVER_PICO_DISPLAY_STM32_SPI_CS_PORT, OPTION_DRIVER_PICO_DISPLAY_STM32_SPI_CS_PIN );
static DriverDioOutSTM32PinConfig_T  dataSelectDriver_( OPTION_DRIVER_PICO_DISPLAY_STM32_SPI_DC_PORT, OPTION_DRIVER_PICO_DISPLAY_STM32_SPI_DC_PIN );
static Driver::DIO::Out              csDriver_( chipSelectDriver_, false );
static Driver::DIO::Out              dcDriver_( dataSelectDriver_, false );
static Driver::DIO::Pwm              pwmDriver_( backlightPwm_ );
static Driver::SPI::STM32::Master    spiDriver_( OPTION_DRIVER_PICO_DISPLAY_STM32_SPI_BUS );



pimoroni::ST7789 st7789_( pimoroni::PicoDisplay::WIDTH, pimoroni::PicoDisplay::HEIGHT, pimoroni::ROTATE_0, false, spiDriver_, csDriver_, dcDriver_, pwmDriver_ );



/////////////////////////////////////////////
void Driver::PicoDisplay::STM32::initialize()
{
    st7789_.start(); 
    rgbLEDDriver_.start();

    driverButtonHalSTM32_initialize( buttonA_.getHandle() );
    driverButtonHalSTM32_initialize( buttonB_.getHandle() );
    driverButtonHalSTM32_initialize( buttonX_.getHandle() );
    driverButtonHalSTM32_initialize( buttonY_.getHandle() );
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
