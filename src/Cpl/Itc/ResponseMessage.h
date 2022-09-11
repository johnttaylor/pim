#ifndef Cpl_Itc_ResponseMessage_h_
#define Cpl_Itc_ResponseMessage_h_
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


#include "Cpl/Itc/AsyncReturnHandler.h"
#include "Cpl/Itc/RequestMessage.h"


///
namespace Cpl {
///
namespace Itc {


/** This template class represents a client response message, which is posted
    to the client's mailbox after the corresponding server request message is
    returned to the client via the server message returnToSender interface. The
    purpose of this class is to provide a standard model for confirmed
    asynchronous messages. The SERVER template argument is the type of the
    server interface that is used by the corresponding server request message.
    The CLIENT class type is the interface which is invoked by the response
    message when the client thread invokes the process routine inherited from
    the Message class.The CLIENT interface must contain a function named
    "response", which returns nothing (void) and takes a single argument, which
    is a reference to this template class type.
 */
template <class CLIENT, class SERVER, class PAYLOAD>
class ResponseMessage : public Message
{
private:
    /// Reference to the client interface whose "response" method will be called
    CLIENT & m_client;

    /// Return handler used to deliver the response
    AsyncReturnHandler              m_rh;

    /// I contain the actual instantiated server request message!
    RequestMessage<SERVER, PAYLOAD>  m_request;

public:
    /// Constructor
    ResponseMessage( CLIENT& client, PostApi& clientsMbox, SERVER& server, PAYLOAD& payload );

    /// Constructor
    ResponseMessage( CLIENT& client, PostApi& clientsMbox, SAP<SERVER>& serverSap, PAYLOAD& payload );

    /// Destructor
    virtual ~ResponseMessage();


public:
    /// See Message
    void process() noexcept;


public:
    /// Returns a reference to the contained server-request-message
    RequestMessage<SERVER, PAYLOAD>& getRequestMsg();

    /// Returns a reference the payload associated with this request/response
    PAYLOAD& getPayload();
};


/////////////////////////////////////////////////////////////////////////////
//                          IMPLEMENATION
/////////////////////////////////////////////////////////////////////////////

template <class CLIENT, class SERVER, class PAYLOAD>
ResponseMessage<CLIENT, SERVER, PAYLOAD>::ResponseMessage( CLIENT& cli, PostApi& cmbox, SERVER& srv, PAYLOAD& payload )
    :m_client( cli ),
    m_rh( cmbox, *this ),
    m_request( srv, payload, m_rh )
{
}

template <class CLIENT, class SERVER, class PAYLOAD>
ResponseMessage<CLIENT, SERVER, PAYLOAD>::ResponseMessage( CLIENT& cli, PostApi& cmbox, SAP<SERVER>& serverSap, PAYLOAD& payload )
    :m_client( cli ),
    m_rh( cmbox, *this ),
    m_request( serverSap, payload, m_rh )
{
}

template <class CLIENT, class SERVER, class PAYLOAD>
ResponseMessage<CLIENT, SERVER, PAYLOAD>::~ResponseMessage()
{
}

template <class CLIENT, class SERVER, class PAYLOAD>
void ResponseMessage<CLIENT, SERVER, PAYLOAD>::process() noexcept
{
    m_client.response( *this );
}

template <class CLIENT, class SERVER, class PAYLOAD>
RequestMessage<SERVER, PAYLOAD>& ResponseMessage<CLIENT, SERVER, PAYLOAD>::getRequestMsg()
{
    return m_request;
}

template <class CLIENT, class SERVER, class PAYLOAD>
PAYLOAD& ResponseMessage<CLIENT, SERVER, PAYLOAD>::getPayload()
{
    return m_request.getPayload();
}

};      // end namespaces
};
#endif  // end header latch
