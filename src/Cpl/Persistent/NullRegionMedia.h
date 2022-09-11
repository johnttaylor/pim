#ifndef Cpl_Persistent_Null_Region_Media_h_
#define Cpl_Persistent_Null_Region_Media_h_
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

#include "Cpl/Persistent/RegionMedia.h"
#include <stdlib.h>
#include <memory.h>


///
namespace Cpl {
///
namespace Persistent {


/** This concrete class provides a 'null' implementation of the RegionMedia
    interface. This class can be used to 'satisfy' persistent storage clients
    when there is no physical persistent storage available on the platform.
 */
class NullRegionMedia : public RegionMedia
{
public:
    /// Constructor.  
    NullRegionMedia( size_t startAddress, size_t regionLength ): RegionMedia( startAddress, regionLength ) {}

public:
    /// See Cpl::Persistent::RegionMedia
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept {}

    /// See Cpl::Persistent::RegionMedia
    void stop() noexcept {}

    /// See Cpl::Persistent::RegionMedia
    bool write( size_t offset, const void* srcData, size_t srcLen ) noexcept { return true; }

    /// See Cpl::Persistent::RegionMedia
    size_t read( size_t offset, void* dstBuffer, size_t bytesToRead ) noexcept { return 0; }
};


};      // end namespaces
};
#endif  // end header latch
