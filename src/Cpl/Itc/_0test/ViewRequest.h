#ifndef ViewRequest_h_
#define ViewRequest_h_
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

#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"

/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class ViewRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<ViewRequest> SAP;

public:
    /// Payload for Message: Subscribing for change notifications
    class AttachPayload
    {
    public:
        ///
        int m_value;

    public:
        ///
        AttachPayload( int v=0 ) :m_value( v ) {};
    };

    /// Message Type: Attach (This message should always be send ASYNCHRONOUSLY)
    typedef Cpl::Itc::RequestMessage<ViewRequest, AttachPayload> AttachMsg;


public:
    /// Payload for Message: Un-subscribing to change notifications
    class DetachPayload
    {
    public:
        ///
        AttachMsg& m_msgToDetach;

    public:
        ///
        DetachPayload( AttachMsg& msgToDetach ) :m_msgToDetach( msgToDetach ) {};
    };

    /// Message Type: Detach (This message should always be send ASYNCHRONOUSLY)
    typedef Cpl::Itc::RequestMessage<ViewRequest, DetachPayload> DetachMsg;


public:
    /// Request: Attach
    virtual void request( AttachMsg& msg ) = 0;

    /// Request: Detach
    virtual void request( DetachMsg& msg ) = 0;

};


class ViewResponseApi
{
public:
    /// Response Message Type: Attach (aka change notification)
    typedef Cpl::Itc::ResponseMessage<ViewResponseApi,
        ViewRequest,
        ViewRequest::AttachPayload> AttachMsg;


    /// Response Message Type: Detach (aka cancel subscription)
    typedef Cpl::Itc::ResponseMessage<ViewResponseApi,
        ViewRequest,
        ViewRequest::DetachPayload> DetachMsg;


public:
    /// Response: AttachMsg
    virtual void response( AttachMsg& msg ) = 0;

    /// Response: AttachMsg
    virtual void response( DetachMsg& msg ) = 0;

};


#endif  // end header latch
