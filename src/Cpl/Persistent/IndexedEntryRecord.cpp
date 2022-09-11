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

#include "IndexedEntryRecord.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Trace.h"
#include <memory.h>

#define SECT_ "Cpl::Persistent"


///
using namespace Cpl::Persistent;

//////////////////////////////////////////////////////
IndexedEntryRecord::IndexedEntryRecord( Chunk&                  entryChunkHandler,
                                        size_t                  singleEntrySizeInBytes,
                                        RegionMedia&            entryRegion,
                                        IndexRecord&            secondaryRecord,
                                        Cpl::Dm::Mp::Uint64&    mpForLatestIndexValue ) noexcept
    : DataRecord( entryChunkHandler )
    , m_mpIndex( mpForLatestIndexValue )
    , m_indexRecord( secondaryRecord )
    , m_entryRegion( entryRegion )
    , m_entryPayloadHandlerPtr( 0 )
    , m_entrySize( singleEntrySizeInBytes + getMetadataLength() + entryChunkHandler.getMetadataLength() )
    , m_maxEntries( entryRegion.getRegionLength() / ( m_entrySize ) )
    , m_maxOffset( ( m_maxEntries - 1 ) * m_entrySize )
{
    CPL_SYSTEM_ASSERT( m_maxEntries >= 1 );
}

void IndexedEntryRecord::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    // Start my parent
    DataRecord::start( myMbox );

    // Start my secondary index record
    m_indexRecord.start( myMbox );

    // Housekeeping
    m_entryTimestamp  = 0;
    m_latestOffset    = 0;
    m_latestTimestamp = 0;

    // Get my head/tail pointers
    m_indexRecord.readFromMedia();  // NOTE: If the Index data is corrupt, then the Index record was 'reset' and the head pointer is set to zero
    verifyIndex();
    m_mpIndex.write( m_latestTimestamp );
}



void IndexedEntryRecord::verifyIndex() noexcept
{
    // Only interested in the timestamp/meta-data
    m_entryPayloadHandlerPtr = 0;

    // Check if the 'latest' record is valid
    m_indexRecord.getLatestOffset( m_latestOffset, m_latestTimestamp );   // Note: The head pointer is guaranteed to be valid (i.e. either valid, or have been reset to zero if/when bad data)
    if ( !readFromMedia( m_latestOffset ) || m_entryTimestamp != m_latestTimestamp )
    {
        // The Latest entry record is corrupt OR there is mismatch in timestamps!
        // Possible causes:
        // 1. Virgin flash
        // 2. Corruption of the entry
        // 3. A new index was written, but the associated entry was not written due to a power fail
        // 4. The 'integrity' of the Ring Buffer has been compromised

        // To Recover: Scan the entire region to derive the latest timestamp from what 
        //             is persistently stored and then update index record in persistent media
        scanAllEntries();

        // Update the index record in persistent storage
        m_indexRecord.setLatestOffset( m_latestOffset, m_latestTimestamp );
        m_indexRecord.writeToMedia();
    }
}

void IndexedEntryRecord::stop() noexcept
{
    // Stop my secondary index record
    m_indexRecord.stop();

    // Stop my Parent
    DataRecord::stop();
}

bool IndexedEntryRecord::processNoValidData() noexcept
{
    // This method gets called if the entry in persistent stored is not-valid and/or is corrupt
    return false;
}

////////////////////////////////////////
bool IndexedEntryRecord::getLatest( Payload& dst, IndexedEntryReader::EntryMarker_T& entryMarker ) noexcept
{
    // Read the latest record
    return getByOffset( m_latestOffset, dst, entryMarker );
}

bool IndexedEntryRecord::getNext( uint64_t                                  newerThan,
                                  const IndexedEntryReader::EntryMarker_T   beginHereMarker,
                                  Payload&                                  dst,
                                  IndexedEntryReader::EntryMarker_T&        entryMarker ) noexcept
{
    // Set the starting offset to on where to begin the search
    size_t offset = incrementOffset( beginHereMarker.mediaOffset );

    // Loop through all possible entries
    for ( size_t i=0; i < m_maxEntries - 1; i++, offset = incrementOffset( offset ) )
    {
        if ( getByOffset( offset, dst, entryMarker ) && m_entryTimestamp > newerThan )
        {
            return true;
        }
    }

    // If I get here, no next entry was found
    return false;
}

bool IndexedEntryRecord::getPrevious( uint64_t                                  olderThan,
                                      const IndexedEntryReader::EntryMarker_T   beginHereMarker,
                                      Payload&                                  dst,
                                      IndexedEntryReader::EntryMarker_T&        entryMarker ) noexcept
{
    // Set the starting offset to on where to begin the search
    size_t offset = decrementOffset( beginHereMarker.mediaOffset );

    // Loop through all possible entries
    for ( size_t i=0; i < m_maxEntries - 1; i++, offset = decrementOffset( offset ) )
    {
        if ( getByOffset( offset, dst, entryMarker ) && m_entryTimestamp < olderThan )
        {
            return true;
        }
    }

    // If I get here, no previous entry was found
    return false;
}

bool IndexedEntryRecord::getByBufferIndex( size_t                             bufferIndex,
                                           Payload&                           dst,
                                           IndexedEntryReader::EntryMarker_T& entryMarker ) noexcept
{
    // Fail request with a bad buffer index
    if ( bufferIndex > getMaxIndex() )
    {
        return false;
    }

    // The 'first' record actually starts at offset_zero + entrySize
    // This means that the index-zero maps to offset: entrySize,  and last index maps to offset: 0
    // So we need to convert the logical index to physical offsets
    size_t offset = ( bufferIndex + 1 ) * m_entrySize;
    if ( offset > m_maxOffset )
    {
        offset = 0;
    }
    return getByOffset( offset, dst, entryMarker );
}


size_t IndexedEntryRecord::getMaxIndex() const noexcept
{
    return m_maxEntries - 1;
}

bool IndexedEntryRecord::getByOffset( size_t                                offset,
                                      Payload&                              dst,
                                      IndexedEntryReader::EntryMarker_T&    entryMarker ) noexcept
{
    // Read the entry
    m_entryPayloadHandlerPtr = &dst;
    if ( !readFromMedia( offset ) )
    {
        // No valid entry 
        CPL_SYSTEM_TRACE_MSG( SECT_, ("getByOffest(): offset=%lu, timestamp=%lu", offset, (uint32_t) m_entryTimestamp ) );
        return false;
    }

    entryMarker.indexValue  = m_entryTimestamp;
    entryMarker.mediaOffset = offset;
    return true;
}

////////////////////////////////////////
bool IndexedEntryRecord::addEntry( const Payload& src ) noexcept
{
    // Increment my head pointer/index
    size_t latestOffset = incrementOffset( m_latestOffset );
    m_entryTimestamp    = m_latestTimestamp + 1;

    // Write the index record
    m_indexRecord.setLatestOffset( latestOffset, m_entryTimestamp );
    if ( !m_indexRecord.writeToMedia() )
    {
        return false;
    }
    m_latestOffset    = latestOffset;
    m_latestTimestamp = m_entryTimestamp;
    m_mpIndex.write( m_latestTimestamp );

    // Set the application payload handler
    m_entryPayloadHandlerPtr = (Payload*) ( &src );

    // Write the entry
    return writeToMedia( m_latestOffset );
}

bool IndexedEntryRecord::clearAllEntries() noexcept
{
    // 'clear' the entire ENTRY Region - 128 bytes at a time
    uint8_t buffer[128] = { 0, };
    bool    result      = true;
    for ( size_t offset=0; offset < m_entryRegion.getRegionLength() && result;  )
    {
        size_t remainingLen = m_entryRegion.getRegionLength() - offset;
        size_t len          = remainingLen < sizeof( buffer ) ? remainingLen : sizeof( buffer );
        result              = m_entryRegion.write( offset, buffer, len );
        offset             += len;
    }

    // Reset my head pointer
    m_latestOffset    = 0;
    m_latestTimestamp = 0;

    // If there was an error -->try our best to recover the actual head pointer
    if ( !result )
    {
        scanAllEntries();
    }

    // Update the index record in persistent storage
    m_indexRecord.setLatestOffset( m_latestOffset, m_latestTimestamp );
    m_indexRecord.writeToMedia();
    m_mpIndex.write( m_latestTimestamp );
    return result;
}

size_t IndexedEntryRecord::incrementOffset( size_t offsetToIncrement ) const noexcept
{
    // Increment the offset
    offsetToIncrement += m_entrySize;

    // Make sure there is still room to store a complete entry AFTER we advance the pointer
    if ( offsetToIncrement > m_maxOffset )
    {
        return 0;
    }
    return offsetToIncrement;
}

size_t IndexedEntryRecord::decrementOffset( size_t offsetToDecrement ) const noexcept
{
    size_t newOffset = offsetToDecrement - m_entrySize;
    if ( newOffset > offsetToDecrement )
    {
        return m_maxOffset;
    }
    return newOffset;
}


////////////////////////////////////////
size_t IndexedEntryRecord::getMetadataLength() const noexcept
{
    return sizeof( m_entryTimestamp );
}

size_t IndexedEntryRecord::getData( void* dst, size_t maxDstLen ) noexcept
{
    // Reject if the destination buffer is too small OR no source payload has been provided
    if ( maxDstLen < getMetadataLength() || m_entryPayloadHandlerPtr == 0 )
    {
        return 0;
    }

    // Plant the timestamp
    memcpy( dst, &m_entryTimestamp, sizeof( m_entryTimestamp ) );
    size_t result = m_entryPayloadHandlerPtr->getData( ( (uint8_t*) dst ) + getMetadataLength(), maxDstLen - getMetadataLength() );
    return result == 0 ? 0 : result + getMetadataLength();
}

bool IndexedEntryRecord::putData( const void* src, size_t srcLen ) noexcept
{
    // Reject if the incoming data is 'short'
    if ( srcLen < getMetadataLength() )
    {
        return false;
    }

    // Extract the timestamp
    memcpy( &m_entryTimestamp, src, sizeof( m_entryTimestamp ) );

    // ONLY Consuming the meta-data/timestamp
    if ( m_entryPayloadHandlerPtr == 0 )
    {
        return true;
    }

    return m_entryPayloadHandlerPtr->putData( ( (const uint8_t*) src ) + getMetadataLength(), srcLen - getMetadataLength() );
}

////////////////////////////////////////
void IndexedEntryRecord::scanAllEntries()
{
    m_latestOffset    = 0;
    m_latestTimestamp = 0;

    for ( size_t offset = 0; offset <= m_maxOffset; offset += m_entrySize )
    {
        if ( readFromMedia( offset ) )
        {
            if ( m_entryTimestamp > m_latestTimestamp || m_latestTimestamp == 0 )
            {
                m_latestTimestamp = m_entryTimestamp;
                m_latestOffset     = offset;
            }
        }
    }
}

