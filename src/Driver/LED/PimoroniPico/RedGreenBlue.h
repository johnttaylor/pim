#ifndef Driver_LED_PimoroniPico_RedGreeBlue_h_
#define Driver_LED_PimoroniPico_RedGreeBlue_h_
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

#include "rgbled.hpp"
#include "pimoroni_common.hpp"
#include "Driver/LED/RedGreenBlue.h"

///
namespace Driver {
///
namespace LED {
///
namespace PimoroniPico {

/** This concrete class is wrapper to the pimoroni RGBLED class
 */
class RedGreenBlue: public Driver::LED::RedGreenBlue, public pimoroni::RGBLED
{
public:
    /// Constructor
    RedGreenBlue( unsigned pin_r, unsigned pin_g, unsigned pin_b, pimoroni::Polarity polarity=pimoroni::Polarity::ACTIVE_LOW, uint8_t brightness=255 )
        : pimoroni::RGBLED( pin_r, pin_g, pin_b, polarity, brightness )
    {
    }

public:
    /// See Driver::LED::RedGreeBlue
    void setRgb( uint8_t redValue, uint8_t greenValue, uint8_t blueValue ) noexcept
    {
        set_rgb( redValue, greenValue, blueValue );
    }
    
    /// See Driver::LED::RedGreeBlue
    void setHsv( float hue, float saturation, float value ) noexcept
    {
        set_hsv( hue, saturation, value );
    }

    /// See Driver::LED::RedGreeBlue
    void setBrightness( uint8_t brightness ) noexcept
    {
        set_brightness( brightness );
    }
};

} // End namespace(s)
}
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
