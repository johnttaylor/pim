#ifndef Cpl_Persistent_Mirrored_Chunk_h_
#define Cpl_Persistent_Mirrored_Chunk_h_
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


#include "Cpl/Persistent/Chunk.h"
#include "Cpl/Persistent/RegionMedia.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Persistent {


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
    /// See Cpl::Persistent::Chunk
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::Chunk
    void stop() noexcept;

    /// See Cpl::Persistent::Chunk
    bool loadData( Payload& dstHandler ) noexcept;

    /// See Cpl::Persistent::Chunk
    bool updateData( Payload& srcHandler ) noexcept;

protected:
    /// Helper method.  If the region is 'corrupt' a transaction ID of zero is returned
    uint64_t getTransactionId( RegionMedia& region, size_t& dataLen );

    /// Helper method. Encapsulates pushing data to the record
    virtual bool pushToRecord(Payload& dstHandler);

    /// Helper method. Encapsulates retrieving data from the record
    virtual bool pullFromRecord(Payload& srcHandler);

    /// Helper method. Encapsulates actions that occur when there is NO VALID data
    virtual void reset();


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
