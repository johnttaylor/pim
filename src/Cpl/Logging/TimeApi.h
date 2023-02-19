#ifndef Cpl_Logging_TimeApi_h_
#define Cpl_Logging_TimeApi_h_
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


#include "colony_config.h"
#include <stdint.h>

/** Default data type for storing a time-stamp value.  The units of the value
    are application specific.
 */
#ifndef CplLoggingTime_T
#define CplLoggingTime_T     uint64_t
#endif

///
namespace Cpl {
///
namespace Logging {

/** This method returns the application current 'time' value that will
    be used to time-stamp each individual log entry. The units of the value
    are application specific.
 */
CplLoggingTime_T now() noexcept;


};      // end namespaces
};
#endif  // end header latch
