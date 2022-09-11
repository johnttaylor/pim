#ifndef Driver_LED_RedGreeBlue_h_
#define Driver_LED_RedGreeBlue_h_
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

#include <stdint.h>

///
namespace Driver {
///
namespace LED {

/** This abstract class defines a basic interface for a single Red-Green-Blue 
    LED where the application can set the individual color values and the overall
    brightness of the LED.
 */
class RedGreenBlue
{
public:
    /** This method is used to set the LED's color value/state by individual Red/Green/Blue color values
     */
    virtual void setRgb( uint8_t redValue, uint8_t greenValue, uint8_t blueValue ) noexcept = 0;

    /** This method is used to set the LED's color value/state by Hue, Saturation, Value
     */
    virtual void setHsv( float hue, float saturation, float value ) noexcept = 0;

    /** This method is used to set the LED's overall brightness
     */
    virtual void setBrightness( uint8_t brightness ) noexcept = 0;

    /// Convenience method to turn the LED off
    inline void setOff() { setRgb( 0, 0, 0 ); }

public:
    /// Virtual destructor to make the compiler happy
    virtual ~RedGreenBlue(){}
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
