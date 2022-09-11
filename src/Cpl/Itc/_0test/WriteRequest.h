#ifndef WriteRequest_h_
#define WriteRequest_h_
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
#include "Cpl/Itc/SAP.h"

/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */
class WriteRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<WriteRequest> SAP;

public:
    /// Payload for Message: Writing
    class WritePayload
    {
    public:
        ///
        int m_value;

    public:
        ///
        WritePayload( int v=0 ) :m_value( v ) {};
    };

    /// Message Type: Write
    typedef Cpl::Itc::RequestMessage<WriteRequest, WritePayload> WriteMsg;



public:
    /// Request: Write
    virtual void request( WriteMsg& msg ) = 0;

};

#endif  // end header latch
