#ifndef Cpl_Dm_Persistent_IndexEntryRecord_h_
#define Cpl_Dm_Persistent_IndexEntryRecord_h_
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
#include "Cpl/Persistent/Chunk.h"
#include "Cpl/Persistent/IndexedEntryReader.h"
#include "Cpl/Persistent/IndexedEntryWriter.h"
#include "Cpl/Persistent/IndexRecord.h"
#include "Cpl/Dm/Mp/Uint64.h"

///
namespace Cpl {
///
namespace Persistent {

/** This concrete class implements the Cpl::Persistent::Record interface
    to store a collection 'entries'. All 'entries' are required to the same/fixed
    length.  A single Chunk handler is used to read/write all of the entries.

    The content of the entry data is defined by a single instance of a application
    specific child class of the Cpl::Persistent::Payload class.

    The individual entries are logically stored in a Ring Buffer.  This means that
    once the Ring Buffer is full (i.e. all of the space available in the
    RegionMedia has been written to) older entries are over written when there
    is request to write a new entry.  In addition a 'secondary record' (with its
    own chunk handler) is used to persistently store the 'head pointer' of
    the Ring Buffer.

    NOTE: The 'secondary record' is wholly managed by the IndexedEntryRecord, i.e.
          the secondary record instance should NOT be directly added to the Record
          Server's list of records to manage.

    A 'index value' is associated with each entry.  The index value is a free
    running counter that is used to determine the relative age between entries.
    The larger an 'index value' is, the newer the entry is.

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
 */
class IndexedEntryRecord : public DataRecord
{
public:
    /// Constructor.
    IndexedEntryRecord( Chunk&                  entryChunkHandler,
                        size_t                  singleEntrySizeInBytes,
                        RegionMedia&            entryRegion,
                        IndexRecord&            secondaryRecord,
                        Cpl::Dm::Mp::Uint64&    mpForLatestIndexValue ) noexcept;

public:
    /// See Cpl::Persistent::Record
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::Record
    void stop() noexcept;

public:
    /// See Cpl::Persistent::Payload (this is for 'entry' payload)
    size_t getData( void* dst, size_t maxDstLen ) noexcept;

    /// See Cpl::Persistent::Payload (this is for 'entry' payload)
    bool putData( const void* src, size_t srcLen ) noexcept;

    /// See Cpl::Persistent::DataRecord
    bool processNoValidData() noexcept;

public:
    /// See Cpl::Persistent::IndexedEntryWriter
    bool addEntry( const Payload& src ) noexcept;

    /// See Cpl::Persistent::IndexedEntryWriter
    bool clearAllEntries() noexcept;

public:
    /// See Cpl::Persistent::IndexedEntryReader
    bool getLatest( Payload& dst, IndexedEntryReader::EntryMarker_T& entryMarker ) noexcept;

    /// See Cpl::Persistent::IndexedEntryReader
    bool getNext( uint64_t                                  newerThan,
                  const IndexedEntryReader::EntryMarker_T   beginHereMarker,
                  Payload&                                  dst,
                  IndexedEntryReader::EntryMarker_T&        entryMarker ) noexcept;

    /// See Cpl::Persistent::IndexedEntryReader
    bool getPrevious( uint64_t                                  olderThan,
                      const IndexedEntryReader::EntryMarker_T   beginHereMarker,
                      Payload&                                  dst,
                      IndexedEntryReader::EntryMarker_T&        entryMarker ) noexcept;

    /// See Cpl::Persistent::IndexedEntryReader
    bool getByBufferIndex( size_t                             bufferIndex,
                           Payload&                           dst,
                           IndexedEntryReader::EntryMarker_T& entryMarker ) noexcept;


    /// See Cpl::Persistent::IndexedEntryReader
    size_t getMaxIndex() const noexcept;

protected:
    /// Helper method: Verifies the 'correctness' of the index/head pointer and 'fixes' the head pointer if it is 'bad'
    void verifyIndex() noexcept;

    /// Helper method: scans all of the 'flash' and returns the newest value found.  If no valid entries found - everything is set to zero
    void scanAllEntries();

    /// Helper method: 'increments' the offset by the size of entry (and handles the 'roll-over' case)
    size_t incrementOffset( size_t offsetToIncrement ) const noexcept;

    /// Helper method: 'decrements' the offset by the size of entry (and handles the 'roll-over' case)
    size_t decrementOffset( size_t offsetToDecrement ) const noexcept;

    /// Helper method: get an entry by its offset
    bool getByOffset( size_t                                offset, 
                      Payload&                              dst,
                      IndexedEntryReader::EntryMarker_T&    entryMarker ) noexcept;

public:
    /// Helper method: returns the size of 'meta-data' that is added to entry
    size_t getMetadataLength() const noexcept;

protected:
    /// Index value of the latest record
    uint64_t                m_latestTimestamp;

    /// Index value to store/push when reading/writing an entry to persistent storage
    uint64_t                m_entryTimestamp;

    /// Model point to report out the Newest index value
    Cpl::Dm::Mp::Uint64&    m_mpIndex;

    /// Index record for storing head/tail pointers
    IndexRecord&            m_indexRecord;

    /// Handle the region used for the entries 
    RegionMedia&            m_entryRegion;
    
    /// Application specific Payload handler for an entry
    Payload*                m_entryPayloadHandlerPtr;

    /// Size, in bytes, of individual Entry 
    size_t                  m_entrySize;

    /// Maximum number of entry that can be stored in the allocated space
    size_t                  m_maxEntries;

    /// The maximum offset value that can be used to store entry without exceeded the allocate entry space
    size_t                  m_maxOffset;

    /// Offset of the latest record;
    size_t                  m_latestOffset;
};



};      // end namespaces
};
#endif  // end header latch