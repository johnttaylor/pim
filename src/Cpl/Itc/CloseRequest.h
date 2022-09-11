#ifndef Cpl_Itc_CloseRequest_h_
#define Cpl_Itc_CloseRequest_h_
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

class CloseRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<CloseRequest> SAP;

public:
    /// Payload for Message: Open
    class ClosePayload
    {
    public:
        /// Pass/Fail result of the class request
        bool m_success;

        /// Optional arguments
        void* m_args;

    public:
        /// Constructor
        ClosePayload( void* args = nullptr ) :m_success( true ), m_args( args ) {}
    };


    /// Message Type: Close
    typedef RequestMessage<CloseRequest, ClosePayload> CloseMsg;


public:
    /// Request: Close
    virtual void request( CloseMsg& msg ) = 0;

public:
    ///
    virtual ~CloseRequest() {}
};



///////////////////////////////////////////////////////////////////////////////
/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: Typically the 'Open Request' is done synchronously.  This interface
          is provided for completeness for the edge case of doing the Open
          Request asynchronously.
 */

class CloseResponse
{
public:
    /// Response Message Type: Open
    typedef ResponseMessage<CloseResponse,
        CloseRequest,
        CloseRequest::ClosePayload> CloseMsg;


public:
    /// Response: OpenMsg
    virtual void response( CloseMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~CloseResponse() {}
};

};      // end namespaces
};
#endif  // end header latch
