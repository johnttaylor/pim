#ifndef Cpl_Itc_AtomicCallback_h_
#define Cpl_Itc_AtomicCallback_h_
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


///
namespace Cpl {
///
namespace Itc {

/** This abstract class defines a call-back interface that clients use
    to issue multiple service requests as a single "atomic" operation.
    The call-back method provides a API that the client can call one
    or more of the methods, one or more times while inside the context
    of the call-back method. All operation(s) invoked inside the call-back
    method run in the server's thread and thus are "atomic" with respect to
    the server.

    TEMPLATE ARGS: ACCESS_API - API that the client uses to accomplish its
                                "Atomic Operation"
 */
template <class ACCESS_API>
class AtomicCallback
{
public:
    /// Virtual destructor
    virtual ~AtomicCallback() {}


public:
    /** This is a call-back method that is implemented by the client.
        This method will run under the server's thread context and
        all method(s) called on the supplied API are atomic with
        respect to the server.
     */
    virtual void execute( ACCESS_API& api ) = 0;
};


};      // end namespaces
};
#endif  // end header latch
