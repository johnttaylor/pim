#ifndef Driver_Tpipe_RxFrameHandler_h_
#define Driver_Tpipe_RxFrameHandler_h_
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

#include "Driver/TPipe/RxFrameHandlerApi.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Container/Key.h"

///
namespace Driver {
///
namespace TPipe {

/** This partially concrete class provides functionality that is common to
    all Frame handlers.  Most notably, this class implements the self-registration
    of a Frame handler. All concrete Frame Handler classes should inherit
    from this class.
 */
class RxFrameHandler: public RxFrameHandlerApi
{
protected:
    /// Constructor
    RxFrameHandler( Cpl::Container::Map<RxFrameHandlerApi>& handlerList, const char* verb ) noexcept;


public:
    /// See Driver::TPipe::RxFrameHandlerApi
    const char* getVerb() const noexcept;

protected:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const noexcept;

protected:
    /// Verb 
    Cpl::Container::KeyLiteralString  m_mapKey;
};


};      // end namespaces
};
#endif  // end header latch
