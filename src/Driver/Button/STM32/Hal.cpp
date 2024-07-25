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

#include "Driver/Button/Hal.h"
#include "Hal.h"


void driverButtonHalSTM32_initialize( Driver_Button_Hal_T buttonHdl )
{
    // TODO: Support configuring WITHOUT using MX tool
}

bool driverButtonHalSTM32_getRawPressState( DriverButtonPinHalSTM32_T pinHandle )
{
    if ( pinHandle.activeLow )
    {
        return HAL_GPIO_ReadPin( pinHandle.port, pinHandle.pin ) ? false : true;
    }
    else 
    {
        return HAL_GPIO_ReadPin( pinHandle.port, pinHandle.pin )? true: false;
    }
}
