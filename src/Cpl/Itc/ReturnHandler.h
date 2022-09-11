#ifndef Cpl_Itc_ReturnHandler_h_
#define Cpl_Itc_ReturnHandler_h_
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

/** This abstract class represents the action that is executed by a server
    thread in response to the server completing the operation requested by
    the SrvMsg, when the server invokes the returnToSender() operation of
    the message. Implementations of this class are the responsibility of
    the client.
 */
class ReturnHandler
{
public:
    /** This abstract operation is invoked when the server has completed processing
        the client request message. Subclass implementations should notify
        the client that the request is complete.
     */
    virtual void rts() noexcept=0;

public:
    /// Virtual destructor
    virtual ~ReturnHandler() {}
};

};      // end namespaces
};
#endif  // end header latch