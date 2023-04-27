#ifndef Driver_DIO_Simulated_InOut_h_
#define Driver_DIO_Simulated_InOut_h_
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

   Configuration Parameters (Config_T):
        pin     - Not used. The maximum number of Signals is defined in InOutModelPoints.h
        blob    - Bits 0-1      - Specifies Pull Up/Pull Down options.
                                    00 = None
                                    01 = Pull-up enabled
                                    10 = Pull-down enabled

                  Bits 1-31     - Reserved
*/

#include <stdint.h>

///
namespace Driver {
///
namespace DIO {
///
namespace Simulated {

/// Magic value for no pull ups.  OR'd with other config options
constexpr size_t INOUT_CFG_NO_PULL_UPDOWN   = 0x00;

/// Magic value for enabling Pull Up resistor.  OR'd with other config options
constexpr size_t INOUT_CFG_PULL_UP          = 0x01;

/// Magic value for enabling Pull Down resistor.  OR'd with other config options
constexpr size_t INOUT_CFG_PULL_DOWN        = 0x02;

/// Mask for the Pull Up/Down configuration option
constexpr size_t INOUT_CFG_PULL_UPDOWN_MASK = 0x03;



} // End namespace(s)
}
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
