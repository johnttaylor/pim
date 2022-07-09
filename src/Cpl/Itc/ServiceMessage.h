#ifndef Cpl_Itc_ServiceMessage_h_
#define Cpl_Itc_ServiceMessage_h_
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

#include "Cpl/Itc/Message.h"
#include "Cpl/Itc/ReturnHandler.h"

///
namespace Cpl {
///
namespace Itc {

/** This class represents a defined message, which is posted to a mailbox-server
    as a request. It includes members which enable the client to be notified when
    the server thread has completed the request.
 */
class ServiceMessage : public Message
{
private:
    /** References the handler which is invoked by the server thread when
        it returns ownership of the message to the client.
     */
    ReturnHandler&  m_rh;

public:
    /// Constructor
    ServiceMessage( ReturnHandler& rh ) noexcept;

    /** This operation is invoked by the server when it has completed the
        operation implemented as a part of this message. Use of this operation
        relinquishes the ownership of the message from the server (which invokes
        the operation) to the client.
     */
    void returnToSender() noexcept;
};


};      // end namespaces
};
#endif  // end header latch
