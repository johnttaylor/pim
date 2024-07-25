#ifndef Driver_AIO_HalSingleInput_h_
#define Driver_AIO_HalSingleInput_h_
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

#include "Driver/AIO/HalSingleInput.h"

/// This method NEVER returns. Note: The caller is expected to call Driver_AIO_HalSingle_setADCSize() before calling runtests()
extern void runtests( DriverAIOHalSingleInput_T handleInput );


/*--------------------------------------------------------------------------*/
#endif  // end header latch
