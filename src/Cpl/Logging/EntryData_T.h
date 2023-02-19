#ifndef Cpl_Logging_EntryData_h_
#define Cpl_Logging_EntryData_h_
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


#include "Cpl/Logging/TimeApi.h"
#include <stdint.h>

/** The size, in bytes, reserved to store the text portion of the log entry.
    The size does NOT include the space reserved for the null terminator
 */
#ifndef OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN
#define OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN             (128+OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT+1+OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT+2)   // Allow space for the textified Category and Message IDs
#endif

/** Maximum number of characters in the application CategoryID's enum symbols
 */
#ifndef OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT
#define OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT     16
#endif

/** Maximum number of characters in the application MessageID's enum symbols
*/
#ifndef OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT
#define OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT      32
#endif


///
namespace Cpl {
///
namespace Logging {

/// Defines the content of the Log entry
struct EntryData_T
{
    CplLoggingTime_T    timestamp;    //!< Time-stamp for the entry
    uint32_t            category;     //!< Category identifier
    uint16_t            msgId;        //!< Message type enumeration identifier.  
    char                msgText[OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN + 1];  //!< The 'text' associated with log entry.
};



};      // end namespaces
};
#endif  // end header latch
