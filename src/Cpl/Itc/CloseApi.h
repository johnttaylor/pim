#ifndef Cpl_Itc_CloseApi_h_
#define Cpl_Itc_CloseApi_h_
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


///
namespace Cpl {
///
namespace Itc {

/** This abstract class defines an close interface.  This close interface
    provides the ability to run an "clean-up/close" routine for a
    ITC server object IN its native thread (i.e. the thread that the
    ITC server object executes its ITC requests in).  The close call
    is a SYNCHRONOUS ITC call.  This has the following effects/requirements:

    1) The caller MUST be in a different thread than the thread of the object
       being closed.

    2) The thread of the caller will block until the ITC server object
       has completed its clean-up (i.e. called returnToSender() on
       the Close Request message).
 */
class CloseApi
{
public:
    /** Synchronously closes/cleans-up the called object. The method returns
        if the close operation was successful.
     */
    virtual bool close( void* args = nullptr ) = 0;


public:
    /// Virtual destructor
    virtual ~CloseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
