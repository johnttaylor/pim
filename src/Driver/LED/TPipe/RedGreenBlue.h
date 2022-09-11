#ifndef Driver_LED_TPipe_RedGreeBlue_h_
#define Driver_LED_TPipe_RedGreeBlue_h_
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

#include "Driver/LED/RedGreenBlue.h"
#include "Driver/TPipe/Tx.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/ElapsedTime.h"

/// Size of my string work buffer (not including the null terminator)
#define DRIVER_LED_TPIPE_WORK_BUFFER_SIZE       128

///
namespace Driver {
///
namespace LED {
///
namespace TPipe {


/** This concrete class implements the Red-Green-Blue LED interface by sending
    the LED state over the TPipe to an external client. The driver assumes the
    following TPipe command format (without leading/trailing SOF/EOF framing characters)

    <DD> <HH:MM:SS.sss> writeRGBLed <led> <r> <g> <b> <bright>
    Where:
         <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <led>               LED identifier/label (with no whitespace allowed)
         <r>                 The 'Red' value (0-255) for the RGB LED
         <g>                 The 'Green' value (0-255) for the RGB LED
         <b>                 The 'Blue' value (0-255) for the RGB LED
         <bright>            Overall brightness for the LED (0-255)
 */
class RedGreeBlue : public Driver::LED::RedGreenBlue
{
public:
    /// Constructor
    RedGreeBlue( Driver::TPipe::Tx& tpipeTransmitter, const char* ledLabel )
        : m_tpipe( tpipeTransmitter )
        , m_ledLabel( ledLabel )
    {
    }

public:
    /// See Driver::LED::RedGreeBlue
    void setRgb( uint8_t redValue, uint8_t greenValue, uint8_t blueValue ) noexcept
    {
        m_red   = redValue;
        m_green = greenValue;
        m_blue  = blueValue;
        sendData();
    }

    /// See Driver::LED::RedGreeBlue
    void setHsv( float hue, float saturation, float value ) noexcept
    {
        // TODO: Need to support
        printf( "Driver::LED::RedGreenBlue::setHsv() NOT SUPPORTED.\n" );
    }


    /// See Driver::LED::RedGreeBlue
    void setBrightness( uint8_t brightness ) noexcept
    {
        m_bright = brightness;
        sendData();
    }


protected:
    /// Helper method to update the LED state
    inline void sendData()
    {
        Cpl::Text::FString< DRIVER_LED_TPIPE_WORK_BUFFER_SIZE> tmp;
        formatMsecTimeStamp( tmp, Cpl::System::ElapsedTime::precision().asFlatTime() );
        tmp.formatAppend( " writeRGBLed %s %u %u %u %u",
                          m_ledLabel,
                          m_red,
                          m_green,
                          m_blue,
                          m_bright );
        m_tpipe.sendCommand( tmp.getString() );
    }

protected:
    /// Reference to the TPipe's transmit interface
    Driver::TPipe::Tx&  m_tpipe;

    /// Label/identifier of the LED
    const char*         m_ledLabel;

    /// Red Value
    uint8_t             m_red;

    /// Green Value
    uint8_t             m_green;

    /// Blue Value
    uint8_t             m_blue;

    /// Brightness
    uint8_t             m_bright;
};

} // End namespace(s)
}
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
