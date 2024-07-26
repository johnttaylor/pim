#ifndef Driver_Button_TPipe_Hal_h_
#define Driver_Button_TPipe_Hal_h_
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
    
    This file defines the concrete implementation for the Button HAL that gets
    its raw inputs via a Driver::TPipe.  The driver assumes the following
    RX Command structure:

    \code

    <verb> <b1> DN|up [<b2> DN|up [..<bN> DN|up]]

    Where:
        <verb>      Command verb.  Default value is 'buttons'
        <b1>        Name of button 1 (no whitespace allowed)
        <b2>        Name of button 2 (no whitespace allowed)
        ...
        <bN>        Name of button N (no whitespace allowed)

    NOTE: The state of All buttons must ALWAYS be sent.
        
        Example:
        buttons A up B up X DN Y up     // Only the X button is pressed
        buttons A DN B DN X up Y up     // Only the A & B buttons are pressed

    \endcode

*/

#include "Driver/Button/Hal.h"
#include "Cpl/Container/Map.h"
#include "Driver/TPipe/RxFrameHandlerApi.h"


/** The maximum number of button/button-names the HAL layer supports
 */
#ifndef OPTION_DRIVER_BUTTON_HAL_TPIPE_MAX_BUTTONS
#define OPTION_DRIVER_BUTTON_HAL_TPIPE_MAX_BUTTONS      32
#endif

/** The maximum size, in bytes, of button name (not including the
    null terminator
 */
#ifndef OPTION_DRIVER_BUTTON_HAL_TPIPE_SIZE_BUTTON_NAME
#define OPTION_DRIVER_BUTTON_HAL_TPIPE_SIZE_BUTTON_NAME 16
#endif

 /** Command verb for the TPipe RX frame handler
 */
#ifndef OPTION_DRIVER_BUTTON_HAL_TPIPE_COMMAND_VERB
#define OPTION_DRIVER_BUTTON_HAL_TPIPE_COMMAND_VERB     "buttons"
#endif

/*-------------- PUBLIC API ------------------------------------------------*/
/** This method is used to register the button driver with the TPipe to 
    receive button events.  This method is called once - NOT per button/button-handle.
 */
void driverButtonHalTPipe_initialize( Cpl::Container::Map<Driver::TPipe::RxFrameHandlerApi>& tpipeRxFrameHandlerList);


/*--------------------------------------------------------------------------*/
#endif  
