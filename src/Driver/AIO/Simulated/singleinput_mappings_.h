#ifdef Driver_Aio_HalSingleInput_h_

#ifndef Driver_AIO_Simulated_halsingleinput_mappings_x_h_
#define Driver_AIO_Simulated_halsingleinput_mappings_x_h_
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
 */

#include "Cpl/Dm/Mp/Uint32.h"


/// Simulated Mapping
#define DriverAIOHalSingleInput_T_MAP               Cpl::Dm::Mp::Uint32&

/// Simulated Mapping
#define Driver_AIO_HalSingleInput_sample_MAP        driverAIOHalSingleInputSimulated_sample

/// Simulated Mapping
#define Driver_AIO_HalSingleInput_setADCSize_MAP    driverAIOHalSingleInputSimulated_setADCSize

/// Simulated specific implementation
inline bool driverAIOHalSingleInputSimulated_sample( Cpl::Dm::Mp::Uint32& mpSrcADCBits, uint32_t& dstADCBits )
{
    return mpSrcADCBits.read( dstADCBits );
}

/// Simulated specific implementation
inline uint8_t driverAIOHalSingleInputSimulated_setADCSize( Cpl::Dm::Mp::Uint32& mpSrcADCBits, uint8_t numADCBits )
{
    return numADCBits;
}

#endif  // end header latch
#endif  // end Interface latch