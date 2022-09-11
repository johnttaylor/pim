#ifndef Cpl_Itc_RequestMessage_h_
#define Cpl_Itc_RequestMessage_h_
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

#include "Cpl/Itc/ServiceMessage.h"
#include "Cpl/Itc/SAP.h"



///
namespace Cpl {
///
namespace Itc {

/** This template class represents a service request message to a particular server.
    The SERVER argument of the template is the abstract interface of the server which
    services this message. This is the interface that the message uses to invoke
    the message operation. By definition, the SERVER must have a member function
    named "request" that returns nothing (void) and has a signature whose argument
    is a reference to the message defined by this template.
 */
template <class SERVER, class PAYLOAD>
class RequestMessage : public ServiceMessage
{
private:
    /// Reference to the server interface whose 'request' will be called
    SERVER & m_srv;

    /// Reference to the request payload
    PAYLOAD&    m_payload;

public:
    /// Constructor
    RequestMessage( SERVER& srv, PAYLOAD& payload, ReturnHandler& returnHandler );

    /// Constructor
    RequestMessage( SAP<SERVER>& sapsrv, PAYLOAD& payload, ReturnHandler& returnHandler );

    /// Destructor
    virtual ~RequestMessage();

public:
    /// See Cpl::Itc::Message
    void process() noexcept;

public:
    /// Returns the payload associated with this request
    PAYLOAD & getPayload();
};


/////////////////////////////////////////////////////////////////////////////
//                          IMPLEMENATION
/////////////////////////////////////////////////////////////////////////////

template <class SERVER, class PAYLOAD>
RequestMessage<SERVER, PAYLOAD>::RequestMessage( SERVER& srv, PAYLOAD& payload, ReturnHandler& returnHandler )
    :ServiceMessage( returnHandler ),
    m_srv( srv ),
    m_payload( payload )
{
}

template <class SERVER, class PAYLOAD>
RequestMessage<SERVER, PAYLOAD>::RequestMessage( SAP<SERVER>& sapsrv, PAYLOAD& payload, ReturnHandler& returnHandler )
    :ServiceMessage( returnHandler ),
    m_srv( sapsrv.getServer() ),
    m_payload( payload )
{
}

template <class SERVER, class PAYLOAD>
RequestMessage<SERVER, PAYLOAD>::~RequestMessage()
{
}

template <class SERVER, class PAYLOAD>
void RequestMessage<SERVER, PAYLOAD>::process() noexcept
{
    m_srv.request( *this );
}

template <class SERVER, class PAYLOAD>
PAYLOAD& RequestMessage<SERVER, PAYLOAD>::getPayload()
{
    return m_payload;
}

};      // end namespaces
};
#endif  // end header latch
