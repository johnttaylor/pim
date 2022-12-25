#ifndef Driver_NV_0test_test_h_
#define Driver_NV_0test_test_h_
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

#include "Driver/NV/Api.h"

/// Executes the tests. A return of zero indicates all test passed
int runtests( Driver::NV::Api& uut,
              size_t           expectedPages,
              size_t           expectedBytesPerPage,
              size_t           expectedTotalSize );


#endif  // end header latch
