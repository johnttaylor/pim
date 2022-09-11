#ifndef Driver_Button_TPipe_mappings_x_h_
#define Driver_Button_TPipe_mappings_x_h_
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


/// TPipe Mapping
#define Driver_Button_Pin_Hal_T_MAP                 const char* 


/// RP2040 Mapping
#define Driver_Button_Hal_getRawPressedState_MAP    driverButtonHalTPipe_getRawPressState


/// TPipe specific implementation for getting the raw button state
bool driverButtonHalTPipe_getRawPressState( const char* buttonName );


#endif  // end header latch
