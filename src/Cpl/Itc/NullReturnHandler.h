#ifndef Cpl_Itc_NullReturnHandler_h_
#define Cpl_Itc_NullReturnHandler_h_
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

#include "Cpl/Itc/ReturnHandler.h"

///
namespace Cpl {
///
namespace Itc {

/** This class implements a Null ReturnHandler. This means the message
    is NEVER returned to the calling thread.
 */
class NullReturnHandler : public ReturnHandler
{
public:
    /// Constructor
    NullReturnHandler();

public:
    /// See ReturnHandler
    void rts() noexcept;
};


};      // end namespaces
};
#endif  // end header latch
