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

#include "Record.h"
#include "Cpl/System/Assert.h"

#define SECT_ "Cpl::Dm::Persistent"


///
using namespace Cpl::Dm::Persistent;

//////////////////////////////////////////////////////
Record::Record( Item_T itemList[], Cpl::Persistent::Chunk& chunkHandler, uint8_t schemaMajorIndex, uint8_t schemaMinorIndex ) noexcept
    : m_items( itemList )
    , m_chunkHandler( chunkHandler )
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

        // Start the chunk handler
        m_chunkHandler.start( myMbox );

        // Load the record's data from persistent storage
        if ( !m_chunkHandler.loadData( *this ) )
        {
            // No valid data -->reset to my defaults
            resetData();
        }

        // Subscribe for change notification 
        for ( unsigned i=0; m_items[i].mpPtr != 0; i++ )
        {
            // Only allocate a subscriber when requested
            if ( m_items[i].observerPtr != CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER )
            {
                // Allocate Subscriber
                m_items[i].observerPtr = new Cpl::Dm::SubscriberComposer<Record, Cpl::Dm::ModelPoint>( myMbox, *this, &Record::dataChanged );
                if ( m_items[i].observerPtr )
                {
                    // subscribe with the current sequence number so there will be NO IMMEDIATE call back
                    m_items[i].mpPtr->genericAttach( *( m_items[i].observerPtr ), m_items[i].mpPtr->getSequenceNumber() );
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
                m_items[i].mpPtr->genericDetach( *( m_items[i].observerPtr ) );
                delete m_items[i].observerPtr;
            }
        }

        // Stop the Chunk Handler
        m_chunkHandler.stop();
    }
}


//////////////////////////////////////////////////////
size_t Record::getData( void* dst, size_t maxDstLen ) noexcept
{
    CPL_SYSTEM_ASSERT( maxDstLen > 2 );

    // Write Schema identifiers
    uint8_t* buffer = (uint8_t*)dst;
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
    const uint8_t* buffer = (const uint8_t*)src;
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

void Record::dataChanged( Cpl::Dm::ModelPoint& point ) noexcept
{
    // NOTE: If the write to the storage media failed -->the RAM contents are still valid so no immediate issue.  However, on the next power cycle the record will be defaulted since the CRC is going to be bad
    m_chunkHandler.updateData( *this );
}