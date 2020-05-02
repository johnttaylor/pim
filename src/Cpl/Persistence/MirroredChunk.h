#ifndef Cpl_Persistence_Mirrored_Chunk_h_
#define Cpl_Persistence_Mirrored_Chunk_h_
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


#include "Cpl/Persistence/Chunk.h"
#include "Cpl/Persistence/RegionMedia.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Persistence {


/** This concrete class implements the Chunk interface by storing two copies
    of the Record's data.  This ensures that if power fails during a write
    operation to persistent media - there will also be a good 'previous' copy
    of the data available.
 */
class MirroredChunk: public Chunk
{
public:
    /// Constructor
    MirroredChunk( RegionMedia& regionA, RegionMedia& regionB );

public:
    /// See Cpl::Persistence::Chunk
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistence::Chunk
    void stop() noexcept;

    /// See Cpl::Persistence::Chunk
    bool loadData( Payload& dstHandler ) noexcept;

    /// See Cpl::Persistence::Chunk
    bool updateData( Payload& srcHandler ) noexcept;

protected:
    /// Helper method.  If the region is 'corrupt' a transaction ID of zero is returned
    uint64_t getTransactionId( RegionMedia& region, size_t& dataLen );

protected:
    /// Region/Media for copy A
    RegionMedia& m_regionA;

    /// Region/Media for copy B
    RegionMedia& m_regionB;

    /// Current Transaction ID (the larger the value - the newer the data)
    uint64_t     m_transId;

    /// Data Length for the region
    size_t       m_dataLen;

    /// Pointer to the current region (i.e. newest read/written region)
    RegionMedia* m_currentRegion;

};


};      // end namespaces
};
#endif  // end header latch
