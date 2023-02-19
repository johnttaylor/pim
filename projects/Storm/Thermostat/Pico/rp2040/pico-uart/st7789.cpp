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

#include "../../app.h"
#include "drivers/st7789/st7789.hpp"
#include "pico/stdlib.h"

// Display driver
pimoroni::ST7789 st7789_( MY_APP_DISPLAY_WIDTH, MY_APP_DISPLAY_HEIGHT, pimoroni::ROTATE_0, false, get_spi_pins( pimoroni::BG_SPI_FRONT ) );
void platform_updateLcd( pimoroni::PicoGraphics& graphics )
{
    st7789_.update( &graphics );
}

void platform_setLcdBacklight( uint8_t value )
{
    st7789_.set_backlight( value );
}
