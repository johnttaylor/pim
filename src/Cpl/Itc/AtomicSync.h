#ifndef Cpl_Itc_AtomicSync_h_
#define Cpl_Itc_AtomicSync_h_
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

#include "Cpl/Itc/AtomicApi.h"
#include "Cpl/Itc/AtomicRequest.h"
#include "Cpl/Itc/SyncReturnHandler.h"


///
namespace Cpl {
///
namespace Itc {


/** This concrete class provides the ITC wrapper implementation for
    calling the "Atomic Api" synchronously.

    TEMPLATE ARGS: ACCESS_API   - API that the client uses to accomplish its
                                  "Atomic Operation"
 */
template <class ACCESS_API>
class AtomicSync : public AtomicRequest<ACCESS_API>,
    public AtomicApi<ACCESS_API>
{
private:
    /// Reference to the server's (my) mailbox
    PostApi & m_mbox;

public:
    /// Constructor
    AtomicSync( PostApi& myMbox ) noexcept;

public:
    /// See AtomicApi
    void executeAtomic( AtomicCallback<ACCESS_API>& clientToCallback );
};


/////////////////////////////////////////////////////////////////////////////
//                      IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////

template <class ACCESS_API>
AtomicSync<ACCESS_API>::AtomicSync( PostApi& myMbox ) noexcept
    :m_mbox( myMbox )
{
}

template <class ACCESS_API>
void AtomicSync<ACCESS_API>::executeAtomic( AtomicCallback<ACCESS_API>& clientToCallback )
{
    typename AtomicRequest<ACCESS_API>::Payload payload( clientToCallback );
    SyncReturnHandler                           srh;
    typename AtomicRequest<ACCESS_API>::ReqMsg  msg( *this, payload, srh );
    m_mbox.postSync( msg );
}



};      // end namespaces
};
#endif  // end header latch



