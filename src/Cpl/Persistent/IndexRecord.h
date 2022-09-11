#ifndef Cpl_Dm_Persistent_IndexRecord_h_
#define Cpl_Dm_Persistent_IndexRecord_h_
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

#include "Cpl/Persistent/DataRecord.h"

///
namespace Cpl {
///
namespace Persistent {

/** This concrete class extends the Cpl::Persistent::DataRecord interface
    to store a latest/oldest 'record index' into a RegionMedia.  This class
    is for tracking the head pointer (so to speak) of a collection of
    IndexedEntryRecords.

    From a logical perspective, IndexedEntryRecords are stored in a Ring Buffer
    and when the buffer is full, the oldest entry/records are overwritten.  The
    head pointer of this logical ring buffer is stored in this class.

    Each entry that is stored has an 'index' value associated with it.  This
    'index' is used to uniquely identify each entry (even across overwritten
    entries) and it is used to identify the relative age between the entries.

    NOTE: The index value for the oldest entry is NOT tracked by the Index Record

    NOTE: This class should not be used/directly access by the Application.  The
          methods are public to facilitate use by the Cpl::Persistent namespace
          ONLY

    NOTE: This class can be extended by an application specific child class to
          add additional information to be stored with the index offsets.

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
 */
class IndexRecord : public DataRecord
{
public:
    /// Constructor.
    IndexRecord( Cpl::Persistent::Chunk& chunkHandler ) noexcept;


public:
    /** This method returns the 'offset-index' (starting from the start of
        RegionMedia used for the entries) of the most recently written
        Indexed Entry.

        Returns true if there is at least 'entry' persistent stored; else
        false is returned;
     */
    virtual bool getLatestOffset( size_t& offset, uint64_t& indexValue ) const noexcept;


public:
    /** This method updates/sets the 'offset-index' (starting from the start of
        RegionMedia used for the entries) of the most recently written
        Indexed Entry.
     */
    virtual void setLatestOffset( size_t offset, uint64_t indexValue ) noexcept;


public:
    /// Helper method: returns the length of the payload
    size_t getPayloadSize() const noexcept;

public:
    /// See Cpl::Persistent::DataRecord
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::DataRecord
    void stop() noexcept;

public:
    /// Default Payload method: payload is the 2 offset-index values
    size_t getData( void* dst, size_t maxDstLen ) noexcept;

    /// Default Payload method: payload is the 2 offset-index values
    bool putData( const void* src, size_t srcLen ) noexcept;

protected:
    /// Default Method: Simply resets both the head/tail pointers to ZERO
    bool processNoValidData() noexcept;


protected:
    /// Index of the latest entry
    uint64_t  m_latestIndex;

    /// Offset for the latest entry
    size_t    m_latest;

    /// Flag that indicates the state of offsets
    bool      m_validLatest;
};



};      // end namespaces
};
#endif  // end header latch