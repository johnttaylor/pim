#ifndef Cpl_Persistent_Private_h_
#define Cpl_Persistent_Private_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include <stdint.h>

/** The number of bytes of the 'work-buffer'
 */
#ifndef OPTION_CPL_PERSISTENT_WORK_BUFFER_SIZE
#define OPTION_CPL_PERSISTENT_WORK_BUFFER_SIZE     1024
#endif


///
namespace Cpl {
///
namespace Persistent {

/** This PACKAGE SCOPED buffer is a singleton that is available as 'work buffer'
    for Chunk and Record instance to use WHEN executing in the RecordServer's
    thread.
 */
extern uint8_t g_workBuffer_[OPTION_CPL_PERSISTENT_WORK_BUFFER_SIZE];


};      // end namespaces
};
#endif  // end header latch