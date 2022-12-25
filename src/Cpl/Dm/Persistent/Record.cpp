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

#include "Record.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/Itc/SyncReturnHandler.h"


#define SECT_ "Cpl::Dm::Persistent"


///
using namespace Cpl::Dm::Persistent;

//////////////////////////////////////////////////////
Record::Record( Item_T                  itemList[],
                Cpl::Persistent::Chunk& chunkHandler,
                uint8_t                 schemaMajorIndex,
                uint8_t                 schemaMinorIndex,
                uint32_t                writeDelayMs,
                uint32_t                maxDelayMs ) noexcept
    : m_items( itemList )
    , m_chunkHandler( chunkHandler )
    , m_delayMs( writeDelayMs )
    , m_maxDelayMs( maxDelayMs )
    , m_timerMarker( 0 )
    , m_major( schemaMajorIndex )
    , m_minor( schemaMinorIndex )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( itemList );
}

Record::~Record()
{
    // Make sure I am stopped (to free any previously allocate memory)
    stop();
}

void Record::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    if ( !m_started )
    {
        m_started = true;

        // Set the timing source (for my delay timer)
        setTimingSource( myMbox );

        // Start the chunk handler
        m_chunkHandler.start( myMbox );

        // Load the record's data from persistent storage
        bool subscribeForChanges = true;
        if ( !m_chunkHandler.loadData( *this ) )
        {
            // No valid data -->reset to my defaults
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Initial loadData() failed. mp[0]=%s", m_items[0].mpPtr->getName()) );
            if ( resetData() )
            {
                updateNVRAM();
            }

            // The Record has requested that NO UPDATES to persistence storage occur -->so we prevent the subscriptions
            else
            {
                subscribeForChanges = false;
            }
        }

        // Record was successfully loaded -->allow for optional child class processing
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Initial loadData() succeeded! mp[0]=%s", m_items[0].mpPtr->getName()) );
            hookProcessPostRecordLoaded();
        }

        // Subscribe for change notification 
        for ( unsigned i=0; m_items[i].mpPtr != 0; i++ )
        {
            // Stop the subscriptions from ever happening
            if ( !subscribeForChanges )
            {
                m_items[i].observerPtr = CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER;
                continue;
            }

            // Only allocate a subscriber when requested
            if ( m_items[i].observerPtr != CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER )
            {
                // Allocate Subscriber
                m_items[i].observerPtr = new Cpl::Dm::SubscriberComposer<Record, Cpl::Dm::ModelPoint>( myMbox, *this, &Record::dataChanged );
                if ( m_items[i].observerPtr )
                {
                    // Subscribe with the current sequence number so there will be NO IMMEDIATE call back
                    m_items[i].mpPtr->genericAttach( *(m_items[i].observerPtr), m_items[i].mpPtr->getSequenceNumber() );
                }
                else
                {
                    Cpl::System::FatalError::logf( "Cpl::Dm::Persistent::Record::start().  Failed to allocate subscriber (i=%u)", i );
                }
            }
        }
    }
}

void Record::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;

        // Cancel subscriptions
        for ( unsigned i=0; m_items[i].mpPtr != 0; i++ )
        {
            // Skip if there is no Subscriber instance
            if ( m_items[i].observerPtr != CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER )
            {
                // subscribe with the current sequence number so there will be NO IMMEDIATE call back
                m_items[i].mpPtr->genericDetach( *(m_items[i].observerPtr) );
                delete m_items[i].observerPtr;
            }
        }

        // Stop the Chunk Handler
        m_chunkHandler.stop();
    }
}

//////////////////////////////////////////////////////
size_t Record::getRecordSize() noexcept
{
    size_t byteCount = 2;
    for ( unsigned i = 0; m_items[i].mpPtr != 0; i++ )
    {
        byteCount += m_items[i].mpPtr->getExternalSize();
    }
    return byteCount;
}

size_t Record::getData( void* dst, size_t maxDstLen ) noexcept
{
    CPL_SYSTEM_ASSERT( maxDstLen > 2 );

    // Write Schema identifiers
    uint8_t* buffer = (uint8_t*) dst;
    *buffer++       = m_major;
    *buffer++       = m_minor;
    maxDstLen      -= 2;

    // Export the Model Point data
    size_t bytesWritten = 2;
    for ( unsigned i = 0; m_items[i].mpPtr != 0; i++ )
    {
        // Check to see if we would overflow the buffer
        size_t mpDataSize = m_items[i].mpPtr->getExternalSize();
        if ( mpDataSize > maxDstLen )
        {
            return 0;
        }

        // Export the data (and check for errors)
        if ( m_items[i].mpPtr->exportData( buffer, maxDstLen ) == 0 )
        {
            return 0;
        }

        // Housekeeping
        maxDstLen    -= mpDataSize;
        bytesWritten += mpDataSize;
        buffer       += mpDataSize;
    }

    return bytesWritten;
}

bool Record::putData( const void* src, size_t srcLen ) noexcept
{
    CPL_SYSTEM_ASSERT( srcLen > 2 );

    // Get schema identifiers
    const uint8_t* buffer = (const uint8_t*) src;
    uint8_t major         = *buffer++;
    uint8_t minor         = *buffer++;
    srcLen               -= 2;

    // Enforce schema rules
    if ( major != m_major || minor != m_minor )
    {
        return schemaChange( major, minor, buffer, srcLen );
    }

    // Import the Model Point data
    for ( unsigned i = 0; m_items[i].mpPtr != 0; i++ )
    {
        // Check to see if there is enough data in the buffer
        size_t mpDataSize = m_items[i].mpPtr->getExternalSize();
        if ( mpDataSize > srcLen )
        {
            return false;
        }

        // Import the data (and check for errors)
        if ( m_items[i].mpPtr->importData( buffer, srcLen ) == 0 )
        {
            return false;
        }

        // Housekeeping
        srcLen -= mpDataSize;
        buffer += mpDataSize;
    }

    return true;
}

void Record::dataChanged( Cpl::Dm::ModelPoint& point, Cpl::Dm::SubscriberApi& observer ) noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Record Changed: mp=%s, timeMarker=%lu", point.getName(), m_timerMarker) );

    // NOTE: The observer instance is PURPOSELY not synchronized with the read 
    //       of the MP data.  This is because there can be multiple MPs that 
    //       make up the Record and it is possible to get change notification 
    //       for all MPs, i.e. change notifications are not atomic to the SET 
    //       of MPs that make up Record.  However, the application can enable 
    //       the 'settling time' feature of the Record to minimize the number  
    //       of 'extra' NVRAM updates.

    // No delay -->update NVRAM immediately 
    if ( m_delayMs == 0 )
    {
        updateNVRAM();
    }

    // Defer/Delay the update to NVRAM
    else
    {
        // If the timer is not running -->then this is the 'first' change notification
        uint32_t now = Cpl::System::ElapsedTime::milliseconds();
        if ( Cpl::System::Timer::count() == 0 )
        {
            m_timerMarker = now;
        }

        // Update NVRAM if the maximum delay time has expired
        if ( Cpl::System::ElapsedTime::expiredMilliseconds( m_timerMarker, m_maxDelayMs, now ) )
        {
            Cpl::System::Timer::stop();
            updateNVRAM();
        }

        // Start my software timer to delay the update to NVRAM
        else
        {
            Cpl::System::Timer::start( m_delayMs );
        }
    }
}

void Record::expired( void ) noexcept
{
    updateNVRAM();
}

void Record::updateNVRAM() noexcept
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("updateNVRAM()") );

    // NOTE: If the write to the storage media failed -->the RAM contents are 
    // still valid so no immediate issue.  However, on the next power cycle the 
    // record will be defaulted since the CRC is going to be bad
    m_chunkHandler.updateData( *this );
}

void Record::request( FlushMsg& msg )
{
    msg.getPayload().m_success = m_chunkHandler.updateData( *this );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Flush Request. mp[0]=%s. success=%d", m_items[0].mpPtr->getName(), msg.getPayload().m_success) );
    msg.returnToSender();
}

void Record::request( EraseMsg& msg )
{
    msg.getPayload().m_success = m_chunkHandler.updateData( *this, 0, true );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Erase Request. mp[0]=%s. success=%d", m_items[0].mpPtr->getName(), msg.getPayload().m_success) );
    msg.returnToSender();
}

bool Record::flush( Cpl::Persistent::RecordServer& myRecordsServer ) noexcept
{
    Cpl::Dm::Persistent::FlushRequest::Payload      payload;
    Cpl::Itc::SyncReturnHandler                     srh;
    Cpl::Dm::Persistent::FlushRequest::FlushMsg     msg( *this, payload, srh );
    myRecordsServer.postSync( msg );
    return payload.m_success;
}

bool Record::erase( Cpl::Persistent::RecordServer& myRecordsServer ) noexcept
{
    Cpl::Dm::Persistent::EraseRequest::Payload    payload;
    Cpl::Itc::SyncReturnHandler                   srh;
    Cpl::Dm::Persistent::EraseRequest::EraseMsg   msg( *this, payload, srh );
    myRecordsServer.postSync( msg );
    return payload.m_success;
}
