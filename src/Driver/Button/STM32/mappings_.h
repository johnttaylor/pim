#ifdef Driver_Button_Hal_h_

#ifndef Driver_Button_STM32_mappings_x_h_
#define Driver_Button_STM32_mappings_x_h_
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

    This file provides the mappings for Platform specific types, etc. for the 
    Driver::Button HAL

 */

#include "Bsp/Api.h"
#include <stdint.h>


/** Type for a button handle
    TODO: Current implementation ASSUMES the pin is configured via ST's MX
          tool (including any pull up/downs).  Action - Add support for
          configuring without using the MX tool
 */
struct DriverButtonPinHalSTM32_T
{
    GPIO_TypeDef*   port;       //!< Port structure for the Pin
    uint16_t        pin;        //!< Pin number (within the port)
    bool            activeLow;  //!< Set to true when the pressed state is ACTIVE_LOW; else set to false for ACTIVE_HIGH

    /// Constructor
    DriverButtonPinHalSTM32_T( GPIO_TypeDef*   portStruct,
                               uint16_t        pinNum,
                               bool            isActiveLow )
        : port( portStruct )
        , pin( pinNum )
        , activeLow( isActiveLow )
    {
    }

    /// Constructor
    DriverButtonPinHalSTM32_T( const DriverButtonPinHalSTM32_T& other )
        : port( other.port )
        , pin( other.pin )
        , activeLow( other.activeLow )
    {
    }
};


/// STM32 Mapping
#define Driver_Button_Pin_Hal_T_MAP                 DriverButtonPinHalSTM32_T  


/// STM32 Mapping
#define Driver_Button_Hal_getRawPressedState_MAP    driverButtonHalSTM32_getRawPressState


/// STM32 specific implementation for getting the raw button state
bool driverButtonHalSTM32_getRawPressState( DriverButtonPinHalSTM32_T pinHandle );


#endif  // end header latch
#endif  // end Interface latch