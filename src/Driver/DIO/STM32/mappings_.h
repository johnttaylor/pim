#if defined(Driver_DIO_Out_h_) || defined(Driver_DIO_In_h_) || defined (Driver_DIO_Pwm_h_)


#ifndef Driver_DIO_STM32_mapping_h_
#define Driver_DIO_STM32_mapping_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Bsp/Api.h"
#include <stdint.h>

/** Structure that defines an Output Pin.
    Currently only supports a Output pin that has been previously configured,
    i.e. using the ST's MX Cube/IDE to configure the output pin.
    TODO: Add support for configuring the pin WITHOUT using the ST's HAL/MX layer
 */
struct DriverDioOutSTM32PinConfig_T
{
    GPIO_TypeDef*       port;       //!< Port structure for the Pin
    uint16_t            pin;        //!< Pin number (within the port)

    /// Constructor
    DriverDioOutSTM32PinConfig_T( GPIO_TypeDef* portDef,
                                  uint16_t      pinNum )
        : port( portDef )
        , pin( pinNum )
    {
    }

    /// Copy Constructor
    DriverDioOutSTM32PinConfig_T( const DriverDioOutSTM32PinConfig_T& other )
        : port( other.port )
        , pin( other.pin )
    {
    }
};

/** Structure that defines an Input Pin.
    Currently only supports a Input pin that has been previously configured,
    i.e. using the ST's MX Cube/IDE to configure the output pin.
    TODO: Add support for configuring the pin WITHOUT using the ST's HAL/MX layer
 */
struct DriverDioInSTM32PinConfig_T
{
    GPIO_TypeDef*   port;       //!< Port structure for the Pin
    uint16_t        pin;        //!< Pin number (within the port)

    /// Constructor
    DriverDioInSTM32PinConfig_T( GPIO_TypeDef*   portStruct,
                                 uint16_t        pinNum )
        : port( portStruct )
        , pin( pinNum )
    {
    }

    /// Constructor
    DriverDioInSTM32PinConfig_T( const DriverDioInSTM32PinConfig_T& other )
        : port( other.port )
        , pin( other.pin )
    {
    }
};

/** Structure that defines an PWM signal & Pin
    Currently only supports a Output pin that has been previously configured,
    i.e. using the ST's MX Cube/IDE to configure the output pin.
    TODO: Add support for configuring the pin WITHOUT using the ST's HAL/MX layer
 */
struct DriverDioPwmSTM32Config_T
{
    TIM_HandleTypeDef*   timerBlock;  //!< Timer block struct
    uint32_t             channelNum;  //!< Channel number (set one of the following (from the SDK) [TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4]

    /// Constructor
    DriverDioPwmSTM32Config_T( TIM_HandleTypeDef* timerInstance, uint8_t channelNumber )
        : timerBlock( timerInstance )
        , channelNum( channelNumber )
    {
    }

    /// Copy Constructor
    DriverDioPwmSTM32Config_T( const DriverDioPwmSTM32Config_T& other )
        : timerBlock( other.timerBlock )
        , channelNum( other.channelNum )
    {
    }
};

/// Platform mapping
#define DriverDioOutPinConfig_T_MAP         DriverDioOutSTM32PinConfig_T

/// Platform mapping
#define DriverDioInPinConfig_T_MAP          DriverDioInSTM32PinConfig_T


/// Platform mapping
#define DriverDioPwmConfig_T_MAP            DriverDioPwmSTM32Config_T

/*--------------------------------------------------------------------------*/
#endif  // end header latch
#endif  // end interface latch