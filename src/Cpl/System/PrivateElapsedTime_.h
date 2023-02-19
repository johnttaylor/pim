#ifndef Cpl_System_elapsedTime_Private_h_
#define Cpl_System_elapsedTime_Private_h_
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

#include "colony_map.h"

/** Platform specific function returns the elapsed time in milliseconds
   
    \b Prototype:
        unsigned long CplSystemElapsedTime_getTimeInMilliseconds();
 */
#define CplSystemElapsedTime_getTimeInMilliseconds          CplSystemElapsedTime_getTimeInMilliseconds_MAP


#endif  // end header latch

