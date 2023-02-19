#ifndef Cpl_System_RP2040_Stdio_h_
#define Cpl_System_RP2040_Stdio_h_
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

    This interface returns the Cpl Io Stream for the for SDK's stdio. 
    
    Note: This interface works BEST when using the SDK's stdio-over-USB because
          its implementation supports software FIFO for the streams (where as
          stdio-over-UART only support the hardware FIFOs)

 */


#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"



///
namespace Cpl {
///
namespace System {
///
namespace RP2040 {


/** This method returns a handle to the stdin stream instance
 */
Cpl::Io::Input& getStdinStream();

/** This method returns a handle to the stdout stream instance
 */
Cpl::Io::Output& getStdoutStream();

}       // end namespace
}
}
#endif  // end header latch