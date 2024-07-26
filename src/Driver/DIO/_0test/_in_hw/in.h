#ifndef Driver_DIO_0test_in_hw_in_h_
#define Driver_DIO_0test_in_hw_in_h_
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

#include "Driver/DIO/In.h"
#include <stdint.h>

/// Executes the tests. A return of zero indicates all test passed
void runtests( Driver::DIO::In& uutIn,
               uint32_t         loopDelayMs );


#endif  // end header latch
