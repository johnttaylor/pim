#ifndef Driver_PicoDisplay_TPipe_h
#define Driver_PicoDisplay_TPipe_h
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
    of the PicoDisplay driver using a 'TPipe' to communicate with an external 
    executable that provides the actual Display/IO.

    Note: The implementation creates a thread to execute the TPipe driver in.

    \code
    TPipe Command format for the LCD Data
    -------------------------------------

    <DD> <HH:MM:SS.sss> writeLCDData <x0> <w> <y0> <h> <hexdata>
    Where:
         <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <x0>                Top/left X coordinate (in pixel coordinates) of the rectangle
         <w>                 Width (in display coordinates) of the rectangle. Note: <w> should always be greater than 0
         <y0>                Top/left Y coordinate (in pixel coordinates) of the rectangle
         <h>                 height (in display coordinates) of the rectangle. Note: <h> should always be greater than 0
         <hexdata>           Pixel data as 'ASCII HEX' String (upper case and with no spaces).  Each PIXEL is one byte 
                             Pixel layout is row, then column:
                                 First Pixel is:   x0, y0
                                 Pixel w is:       x0+w, y0
                                 Pixel w+1 is:     x0, y0+1
                                 Pixel (h*w) is:   x1, y1

    NOTE: Color/Pixel size RGB322 color resolution


    <DD> <HH:MM:SS.sss> updateLCD
    Where:
         <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         
    NOTE: The simulator makes a copy of the 'screen buffer' and ONLY sends 'deltas' to the simulated
          display.  This has significant positive impact on the performance of 'display' on the simulator
 
    \endcode
 */

#include "colony_config.h"
#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"
#include "Driver/TPipe/Pipe.h"

/** The size, in bytes, of the receive frame buffer used for the TPipe
    connection
 */
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIPE_RX_FRAME_HANDLER_SIZE
#define OPTION_DRIVER_PICO_DISPLAY_TPIPE_RX_FRAME_HANDLER_SIZE     256
#endif

/// Symbolic name for button: A
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_A_NAME
#define OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_A_NAME      "A"
#endif

/// Symbolic name for button: B
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_B_NAME
#define OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_B_NAME      "B"
#endif

/// Symbolic name for button: X
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_X_NAME
#define OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_X_NAME      "X"
#endif

/// Symbolic name for button: Y
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_Y_NAME
#define OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_Y_NAME      "Y"
#endif

/// Symbolic name for the RGB LED
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIPE_RGB_LED_NAME
#define OPTION_DRIVER_PICO_DISPLAY_TPIPE_RGB_LED_NAME       "theOne"
#endif

/// The width, in pixels, of the Display
#ifndef OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH
#define OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH          240
#endif

/// The height, in pixels, of the Display
#ifndef OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT
#define OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT         135
#endif

/// The TPipe SOF character
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_SOF
#define OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_SOF           '^'
#endif

/// The TPipe EOF character
#ifndef OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_EOF
#define OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_EOF           ';'
#endif

///
namespace Driver {
///
namespace PicoDisplay {
///
namespace TPipe {

/** This method initializes the composite driver and provides the IO streams
    it uses to communicate with the external executable
 */
void initialize( Cpl::Io::Input & tpipeInStream, Cpl::Io::Output & tpipeOutStream ) noexcept;

/** This method returns a handle to the underly TPipe instance
 */
Driver::TPipe::Pipe& tpipe() noexcept;

} // End namespace(s)
}
}


#endif // end header latch
