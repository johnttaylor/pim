#ifndef Cpl_Logging_Private_h_
#define Cpl_Logging_Private_h_
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

    This file contains private (i.e. the application SHOULD never call this
    method directory for the logging framework
*/

#include <stdint.h>
#include <stdarg.h>

///
namespace Cpl {
///
namespace Logging {


/** This method is used to create the log entry and insert into the entry queue
 */
void createAndAddLogEntry_( uint32_t    category, 
                            const char* catIdText, 
                            uint16_t    msgId, 
                            const char* msgIdText, 
                            const char* format, 
                            va_list     ap ) noexcept;



};      // end namespaces
};
#endif  // end header latch
