#ifndef Cpl_Dm_Persistent_EraseRequest_h_
#define Cpl_Dm_Persistent_EraseRequest_h_
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

///
namespace Cpl {
///
namespace Dm {
///
namespace Persistent {


/** This abstract class define ITC message type and payload for the application
    to request invalidating/corrupting the Record in persistent storage (i.e.
    logically erase the record in persistent storage)

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class EraseRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<EraseRequest> SAP;

public:
    /// Payload for Message 
    class Payload
    {
    public:
        /** Erase results (response field)
            true  = the Record was successfully erased/invalidate in persistent storage
            false = an error occurred and the state of record in persistent storage is unknown
         */
        bool m_success;

    public:
        /// Constructor. 
        Payload() :m_success( false ) {}
    };


public:
    /// Message Type:
    typedef Cpl::Itc::RequestMessage<EraseRequest, Payload> EraseMsg;

    /// Request: Erase message
    virtual void request( EraseMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~EraseRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a Erase message.

    The Application is responsible for implementing the response method(s).
 */
class EraseResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<EraseResponse,
                                      EraseRequest,
                                      EraseRequest::Payload> EraseMsg;

public:
    /// Response
    virtual void response( EraseMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~EraseResponse() {}
};


};      // end namespaces
};
};
#endif  // end header latch
