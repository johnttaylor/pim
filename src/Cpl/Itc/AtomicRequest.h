#ifndef Cpl_Itc_AtomicRequest_h_
#define Cpl_Itc_AtomicRequest_h_
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

#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Cpl/Itc/AtomicCallback.h"
#include "Cpl/Itc/SyncReturnHandler.h"

///
namespace Cpl {
///
namespace Itc {

/** This concrete class define the 'Atomic' Message request that allows
    a client to invoke multiple operation on a server/service as one
    single atomic operation.

    The server typically inherits this class and implements the 'ACCESS_API'.
    The client is responsible for implementing the AtomicCallback interface.

    TEMPLATE ARGS: ACCESS_API - API that the client uses to accomplish its
                                "Atomic Operation"
 */
template <class ACCESS_API>
class AtomicRequest : public ACCESS_API
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP< Cpl::Itc::AtomicRequest<ACCESS_API> > SAP;


public:
    /// Payload for Message
    class Payload
    {
    private:
        /// Reference the client defined atomic operation
        AtomicCallback<ACCESS_API>&   m_clientCb;

    public:
        /// Constructor
        Payload( AtomicCallback<ACCESS_API>& clientToCallback ) noexcept:m_clientCb( clientToCallback ) {};

        /// Returns the reference to the client atom
        AtomicCallback<ACCESS_API>&   getClient() noexcept { return m_clientCb; }
    };

    /// Message Type:
    typedef RequestMessage<AtomicRequest, Payload> ReqMsg;


public:
    /// Request
    virtual void request( ReqMsg& msg )
    {
        msg.getPayload().getClient().execute( *this );
        msg.returnToSender();
    }
};



///////////////////////////////////////////////////////////////////////////////
/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    TEMPLATE ARGS: ACCESS_API   - API that the client uses to accomplish its
                                  "Atomic Operation"

    NOTE: Typically the 'Atomic Request' is done synchronously.  This interface
          is provided for completeness for the edge case of doing the Atomic
          Request asynchronously.
 */
template <class ACCESS_API>
class AtomicResponse
{
public:
    /// Response Message Type: Open
    typedef ResponseMessage<AtomicResponse<ACCESS_API>,
        AtomicRequest<ACCESS_API>,
        typename AtomicRequest<ACCESS_API>::Payload> RspMsg;


public:
    /// Response: OpenMsg
    virtual void response( RspMsg& msg ) = 0;

public:
    /// Virtual destructor
    virtual ~AtomicResponse() {}
};


};      // end namespaces
};
#endif  // end header latch
