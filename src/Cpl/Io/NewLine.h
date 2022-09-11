#ifndef Cpl_Io_NewLine_h_
#define Cpl_Io_NewLine_h_
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
#include "colony_config.h"


/// The standard newline is "\n", but allow the application to override/change it
#ifndef OPTION_CPL_IO_NEW_LINE_STANDARD
#define OPTION_CPL_IO_NEW_LINE_STANDARD           "\n"
#endif

/// The application must provide/map the native newline character
#define CPL_IO_NEW_LINE_NATIVE                  CPL_IO_NEW_LINE_NATIVE_MAP     



/// 
namespace Cpl {
/// 
namespace Io {


/** This interface defines the 'newline' character(s).  There is a
    standardized (within Colony.*) newline character which is "\n". In addition
    there is a native (to target's OS) newline character(s).  The recommendation
    is to build your application to ALWAYS use the 'standardized' newline
    character and then use the Colony's Late-Header pattern (aka colony_config.h)
    to customize (if required) the actual newline for the application's
    platform.
 */
class NewLine
{
public:
    /** This method returns the Colony standardized newline character(s)
     */
    static const char* standard( void ) { return OPTION_CPL_IO_NEW_LINE_STANDARD; }



    /** This methods returns the target's native newline character(s).
        NOTE: Use of this method is discouraged - see comments in class
              description.
     */
    static const char* native( void ) { return CPL_IO_NEW_LINE_NATIVE; }
};


};      // end namespaces
};
#endif  // end header latch

