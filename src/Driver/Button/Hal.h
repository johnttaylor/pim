#ifndef Driver_Button_Hal_h_
#define Driver_Button_Hal_h_
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
    
    This file defines a hardware abstraction layer (HAL) for accessing a
    digital Input that is being used for a button.

	NOTE: The HAL layer is defined 'just-in-time', i.e. when adding a new
          driver/functionality and new HAL supported is needed - then this
          interface is updated.

    NOTE: NO 'Initialization' method is provided/defined in this interface -
          this is intentional! The initialization left to the target's platform
          specific code and/or BSP.  The application is RESPONSIBLE for making 
          sure that the platform specific initialization happens BEFORE any of 
          the drivers are started/used.  
*/

#include "colony_map.h"


/*-------------- PUBLIC API ------------------------------------------------*/
/** This data type defines the platform specific 'handle' to a pin.
 */
#define Driver_Button_Hal_T                     Driver_Button_Pin_Hal_T_MAP



/*-------------- PUBLIC API ------------------------------------------------*/
/** This method returns the current/raw state of pin.  A true value indicates
    the button is in its 'Pressed' state; else it is in the 'Un-Pressed' state.

    Prototype:
        bool Driver_Button_Hal_getRawPressedState( Driver_Button_Hal_T hdl );
 */
#define Driver_Button_Hal_getRawPressedState    Driver_Button_Hal_getRawPressedState_MAP





/*--------------------------------------------------------------------------*/
#endif  
