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

#include "Driver/Button/Hal.h"
#include "pico/stdlib.h"

void driverButtonHalRP2040_initialize( Driver_Button_Hal_T buttonHdl )
{
    gpio_set_function( buttonHdl.pinId, GPIO_FUNC_SIO );
    gpio_set_dir( buttonHdl.pinId, GPIO_IN );
    if ( buttonHdl.activeLow ) 
    {
        gpio_pull_up( buttonHdl.pinId );
    }
    else 
    {
        gpio_pull_down( buttonHdl.pinId );
    }
}

bool driverButtonHalRP2040_getRawPressState( Driver_Button_Pin_Hal_RP2040_T pinHandle )
{
    if ( pinHandle.activeLow )
    {
        return !gpio_get( pinHandle.pinId );
    }
    else 
    {
        return gpio_get( pinHandle.pinId );
    }
}