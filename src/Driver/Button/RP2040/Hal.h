#ifndef Driver_Button_RP2040_Hal_h_
#define Driver_Button_RP2040_Hal_h_
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
    
    This file defines the concrete implementation for the Butotn HAL running on 
    Raspberry PI RP2040 MCU

*/

#include "Driver/Button/Hal.h"


/*-------------- PUBLIC API ------------------------------------------------*/
/** This method is used to initialize the GPIO for the pin/configuration specified
    by 'buttonHdl'
 */
void driverButtonHalRP2040_initialize( Driver_Button_Hal_T buttonHdl );


/*--------------------------------------------------------------------------*/
#endif  
