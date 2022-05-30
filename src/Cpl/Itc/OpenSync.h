#ifndef Cpl_Itc_OpenSync_h_
#define Cpl_Itc_OpenSync_h_
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

#include "Cpl/Itc/OpenApi.h"
#include "Cpl/Itc/OpenRequest.h"


///
namespace Cpl {
///
namespace Itc {


/** This partially concrete class implements the synchronous ITC open
    call for the OpenApi.  The subclass is still required to implement the ITC
    request() method.
 */
class OpenSync : public OpenApi,
    public OpenRequest
{
protected:
    /// Mailbox (of the server implementing the ITC request() method
    PostApi & m_mbox;


public:
    /// Constructor
    OpenSync( PostApi& myMbox ) noexcept;


public:
    /// See OpenApi
    bool open( void* args = nullptr );
};



};      // end namespaces
};
#endif  // end header latch
