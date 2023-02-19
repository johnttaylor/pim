#ifndef Driver_Button_RP2040_mappings_x_h_
#define Driver_Button_RP2040_mappings_x_h_
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

/// Type for a button handle
struct Driver_Button_Pin_Hal_RP2040_T{
    unsigned pinId;         //!< SDK pin identifier
    bool     activeLow;     //!< Set to true when the pressed state is ACTIVE_LOW; else set to false for ACTIVE_HIGH
};

/// RP2040 Mapping
#define Driver_Button_Pin_Hal_T_MAP                 Driver_Button_Pin_Hal_RP2040_T  


/// RP2040 Mapping
#define Driver_Button_Hal_getRawPressedState_MAP    driverButtonHalRP2040_getRawPressState


/// RP2040 specific implementation for getting the raw button state
bool driverButtonHalRP2040_getRawPressState( Driver_Button_Pin_Hal_RP2040_T pinHandle );


#endif  // end header latch
