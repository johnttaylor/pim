#ifndef Cpl_Persistent_NVAdapter_h_
#define Cpl_Persistent_NVAdapter_h_
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
#include "Driver/NV/Api.h"


///
namespace Cpl {
///
namespace Persistent {

/** This concrete class implements the RegionMedia interface using the 
    Driver::NV::Api interface. 
 */
class NVAdapter : public RegionMedia
{
public:
    /** Constructor.  
     */
    NVAdapter( Driver::NV::Api& lowLevelDriver, size_t regionStartAddress, size_t regionLen ) noexcept;

public:
    /// See Cpl::Persistent::RegionMedia
    void start( Cpl::Dm::MailboxServer& myMbox_notUsed ) noexcept;

    /// See Cpl::Persistent::RegionMedia
    void stop() noexcept;

    /// See Cpl::Persistent::RegionMedia
    bool write( size_t offset, const void* srcData, size_t srcLen ) noexcept;

    /// See Cpl::Persistent::RegionMedia
    size_t read( size_t offset, void* dstBuffer, size_t bytesToRead ) noexcept;


protected:
    /// Handle to the low level driver that does all of the work
    Driver::NV::Api& m_driver;
};

};      // end namespaces
};
#endif  // end header latch