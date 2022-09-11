#ifndef Cpl_Itc_AtomicApi_h_
#define Cpl_Itc_AtomicApi_h_
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

#include "Cpl/Itc/AtomicCallback.h"

///
namespace Cpl {
///
namespace Itc {

/** This abstract class provides an interface to allow a client
    to call the server synchronously to have the server invoke
    the client's atomic operation.

    TEMPLATE ARGS:  ACCESS_API   - API that the client uses to accomplish its
                                   "Atomic Operation"
 */

template <class ACCESS_API>
class AtomicApi
{
public:
    /** This method initiates (via sync ITC) the process of invoking
        an Atomic Operation.  The clientToCallback is a reference to the
        client's interface that performs the atomic "operation".
     */
    virtual void executeAtomic( AtomicCallback<ACCESS_API>& clientToCallback ) = 0;

public:
    /// Virtual Destructor
    virtual ~AtomicApi() {}
};



};      // end namespaces
};
#endif  // end header latch



