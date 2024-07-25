#if defined(Driver_DIO_Out_h_) || defined(Driver_DIO_In_h_) || defined (Driver_DIO_Pwm_h_)

#ifndef Driver_DIO_Simulated_mapping_h_
#define Driver_DIO_Simulated_mapping_h_
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

#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/Uint32.h"


/// Platform mapping
#define DriverDioOutPinConfig_T_MAP         Cpl::Dm::Mp::Bool&

/// Platform mapping
#define DriverDioInPinConfig_T_MAP          Cpl::Dm::Mp::Bool&

/// Platform mapping
#define DriverDioPwmConfig_T_MAP            Cpl::Dm::Mp::Uint32&

/*--------------------------------------------------------------------------*/
#endif  // end header latch
#endif  // end interface latch