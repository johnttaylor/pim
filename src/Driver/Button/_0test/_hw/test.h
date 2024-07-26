#ifndef Driver_Button_0test_hw_h_
#define Driver_Button_0test_hw_h_
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

#include "Driver/Button/PolledDebounced.h"

/// This method NEVER returns
extern void runtests( Driver_Button_Hal_T& hal1,
                      unsigned             numConsecutiveCounts1,
                      Driver_Button_Hal_T& hal2,
                      unsigned             numConsecutiveCounts2 );


/*--------------------------------------------------------------------------*/
#endif  // end header latch
