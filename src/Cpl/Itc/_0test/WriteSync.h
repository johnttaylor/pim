#ifndef WriteSync_h_
#define WriteSync_h_
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

#include "Cpl/Itc/_0test/WriteApi.h"
#include "Cpl/Itc/_0test/WriteRequest.h"


/** This partially concrete class implements the synchronous ITC open
    call for the OpenApi.  The subclass is still required to implement the ITC
    request() method.
 */
class WriteSync : public WriteApi,
    public WriteRequest
{
protected:
    /// Mailbox (of the server implementing the ITC request() method
    Cpl::Itc::PostApi& m_mbox;

public:
    /// Constructor
    WriteSync( Cpl::Itc::PostApi& myMbox );


public:
    /// See WriteApi
    void write( int newValue ) noexcept;
};



#endif  // end header latch
