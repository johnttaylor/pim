#ifndef Cpl_Dm_Persistent_FlushRequest_h_
#define Cpl_Dm_Persistent_FlushRequest_h_
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
namespace Dm {
///
namespace Persistent {


/** This abstract class define ITC message type and payload for the application
    to request flushing the Record to persistent storage

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class FlushRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<FlushRequest> SAP;

public:
    /// Payload for Message: GetLatest
    class Payload
    {
    public:
        /** Flush results (response field)
            true  = the Record was successfully written to persistent storage
            false = an error occurred and the state of record in persistent storage is unknown
         */
        bool m_success;

    public:
        /// Constructor. 
        Payload() :m_success( false ) {}
    };


public:
    /// Message Type: GetLatest
    typedef Cpl::Itc::RequestMessage<FlushRequest, Payload> FlushMsg;

    /// Request: Flush message
    virtual void request( FlushMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~FlushRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a Flush message.

    The Application is responsible for implementing the response method(s).
 */
class FlushResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<FlushResponse,
                                      FlushRequest,
                                      FlushRequest::Payload> FlushMsg;

public:
    /// Response
    virtual void response( FlushMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~FlushResponse() {}
};


};      // end namespaces
};
};
#endif  // end header latch
