#ifndef Driver_I2C_Master_0test_master_h_
#define Driver_I2C_Master_0test_master_h_
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

#include "Driver/I2C/Master.h"

/// This method NEVER returns
void runtests( Driver::I2C::Master& uut, uint8_t device7BitAddress );


/*--------------------------------------------------------------------------*/
#endif  // end header latch
