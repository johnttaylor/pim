#ifndef Cpl_Itc_OpenRequest_h_
#define Cpl_Itc_OpenRequest_h_
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

#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"

///
namespace Cpl {
///
namespace Itc {

/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */

class OpenRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<OpenRequest> SAP;

public:
    /// Payload for Message: Open
    class OpenPayload
    {
    public:
        /// Pass/Fail result of the open request
        bool m_success;

        /// Optional arguments
        void* m_args;
    public:
        /// Constructor
        OpenPayload( void* args = nullptr ):m_success( true ), m_args(args) {}
    };


    /// Message Type: Open
    typedef RequestMessage<OpenRequest, OpenPayload> OpenMsg;

public:
    /// Request: Open
    virtual void request( OpenMsg& msg ) = 0;

public:
    ///
    virtual ~OpenRequest() {}
};



///////////////////////////////////////////////////////////////////////////////
/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: Typically the 'Open Request' is done synchronously.  This interface
          is provided for completeness for the edge case of doing the Open
          Request asynchronously.
 */
class OpenResponse
{
public:
    /// Response Message Type: Open
    typedef ResponseMessage<OpenResponse,
        OpenRequest,
        OpenRequest::OpenPayload> OpenMsg;


public:
    /// Response: OpenMsg
    virtual void response( OpenMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~OpenResponse() {}
};


};      // end namespaces
};
#endif  // end header latch
