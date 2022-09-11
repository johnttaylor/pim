#ifndef Cpl_Io_InputOutput_h_
#define Cpl_Io_InputOutput_h_
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

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"

///
namespace Cpl {
///
namespace Io {



/** This abstract class defines a interface for operating on an
    input-output stream (example of a stream is socket connection).
 */
class InputOutput : public Input,
    public Output
{
};


};      // end namespaces
};
#endif  // end header latch
