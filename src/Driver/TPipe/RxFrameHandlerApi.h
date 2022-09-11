#ifndef Driver_Tpipe_RxFrameHandlerApi_h_
#define Driver_Tpipe_RxFrameHandlerApi_h_
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

#include "Cpl/Container/MapItem.h"

///
namespace Driver {
///
namespace TPipe {

/** This abstract class defines the 'Received Frame Handler' interface for the
    TPipe. The frame handlers are essentially callback that are executed when
    a valid frame is received.  The frame handler's execute in the TPipe's thread.
    It is the Application RESPONSIBLE for ensuring proper thread-safe inside
    the frame handlers.
 */
class RxFrameHandlerApi: public Cpl::Container::MapItem
{
public:
    /** This method is execute when an valid frame is received AND the frame
        begins with the frame handler' verb (as defined by its getVerb() method).

        The frame is passed via 'decodedFrameText' - which is a null terminated
        string.  The frame handler is ALLOWED to modify the frame contents (e.g.
        destructive tokenizing)
     */
    virtual void execute( char* decodedFrameText ) noexcept = 0;


public:
    /// This method returns the frame handler's verb string
    virtual const char* getVerb() const noexcept = 0;

public:
    /// Virtual destructor
    virtual ~RxFrameHandlerApi() {}
};


};      // end namespaces
};
#endif  // end header latch
