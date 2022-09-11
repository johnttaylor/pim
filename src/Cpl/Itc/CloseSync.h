#ifndef Cpl_Itc_CloseSync_h_
#define Cpl_Itc_CloseSync_h_
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

#include "Cpl/Itc/OpenSync.h"
#include "Cpl/Itc/CloseRequest.h"
#include "Cpl/Itc/CloseApi.h"

///
namespace Cpl {
///
namespace Itc {

/** This partially concrete class implements the synchronous ITC close
    call for the CloseApi.  The subclass is still required to implement the ITC
    request() method.

    NOTE: This class is derived from OpenSync -->if you support a close(),
          call, you better have had an open() call.
 */
class CloseSync : public OpenSync,
    public CloseApi,
    public CloseRequest
{
public:
    /// Constructor
    CloseSync( PostApi& myMbox ) noexcept;


public:
    /// See CloseSync
    bool close( void* args = nullptr );
};




};      // end namespaces
};
#endif  // end header latch
