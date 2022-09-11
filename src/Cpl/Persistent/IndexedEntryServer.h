#ifndef Cpl_Dm_Persistent_IndexEntryServer_h_
#define Cpl_Dm_Persistent_IndexEntryServer_h_
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

#include "colony_config.h"
#include "Cpl/Persistent/IndexedEntryRecord.h"
#include "Cpl/Persistent/IndexedEntryRequests.h"
#include "Cpl/Container/RingBufferMP.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/SubscriberComposer.h"


/** Maximum number of entries that can be written as the result of single
    change notification
 */
#ifndef OPTION_CPL_PERSISTENT_INDEXED_ENTRY_SERVER_MAX_BATCH_WRITE
#define OPTION_CPL_PERSISTENT_INDEXED_ENTRY_SERVER_MAX_BATCH_WRITE      4
#endif


///
namespace Cpl {
///
namespace Persistent {

/** This concrete template class implements the ITC messaging (and Model Point
    monitoring) needed to provide a thread-safe/asynchronous interface for
    reading/writing 'Indexed Entries'

    Template Args:
        ENTRY:=   The Application specific class that defines an 'entry'

 */
template <class ENTRY>
class IndexedEntryServer : public Cpl::Itc::CloseSync,
    public GetLatestRequest,
    public GetNextRequest,
    public GetPreviousRequest,
    public GetByBufferIndexRequest,
    public ClearAllEntriesRequest
{
public:
    /// Constructor.
    IndexedEntryServer( Cpl::Dm::MailboxServer&                 myMbox,
                        IndexedEntryRecord&                     indexedEntryRecord,
                        Cpl::Container::RingBufferMP<ENTRY>&    incomingEntriesBuffer ) noexcept
        : Cpl::Itc::CloseSync( myMbox )
        , m_record( indexedEntryRecord )
        , m_observerElementCount( *( (Cpl::Dm::EventLoop*) &myMbox ), *this, &IndexedEntryServer::elementCountChanged )
        , m_addBuffer( incomingEntriesBuffer )
        , m_opened( false )
    {
    }


public:
    /// This method starts the server (See Cpl::Itc::OpenSync)
    void request( OpenMsg& msg )
    {
        if ( !m_opened )
        {
            m_opened = true;
            m_addBuffer.m_mpElementCount.attach( m_observerElementCount );
        }

        msg.returnToSender();
    }

    /// This method stops the server (See Cpl::Itc::CloseSync)
    void request( CloseMsg& msg )
    {
        if ( m_opened )
        {
            m_opened = false;
            m_addBuffer.m_mpElementCount.detach( m_observerElementCount );
        }

        msg.returnToSender();
    }

    /// Element Count Change notification 
    void elementCountChanged( Cpl::Dm::Mp::Uint32& mp ) noexcept
    {
        uint32_t count;
        if ( mp.read( count ) && count > 0 )
        {
            // Drain the buffer (but limit how many adds at one time) and write the entries to persistent storage
            unsigned iterations = 0;
            ENTRY entry;
            while ( iterations < OPTION_CPL_PERSISTENT_INDEXED_ENTRY_SERVER_MAX_BATCH_WRITE && m_addBuffer.remove( entry ) )
            {
                hookAddingEntry( entry );
                m_record.addEntry( entry );
                iterations++;
            }
        }
    }

protected:
    /// Hook to allow a child class processing when an entry is added. This method is called just PRIOR to writing the data to persistent storage
    virtual void hookAddingEntry( ENTRY& src )
    {
        // Default is to do nothing
    }

public:
    /// See Cpl::Persistent::GetLatestRequest
    void request( GetLatestMsg& msg )
    {
        GetLatestRequest::Payload& payload = msg.getPayload();
        payload.m_success                  = m_record.getLatest( payload.m_entryDst, payload.m_markerEntryRetreived );
        msg.returnToSender();
    }

    /// See Cpl::Persistent::GetNextRequest
    void request( GetNextMsg& msg )
    {
        GetNextRequest::Payload& payload = msg.getPayload();
        payload.m_success                = m_record.getNext( payload.m_newerThan, payload.m_beginHereMarker, payload.m_entryDst, payload.m_markerEntryRetreived );
        msg.returnToSender();
    }


    /// See Cpl::Persistent::GetPreviousRequest
    void request( GetPreviousMsg& msg )
    {
        GetPreviousRequest::Payload& payload = msg.getPayload();
        payload.m_success                    = m_record.getPrevious( payload.m_olderThan, payload.m_beginHereMarker, payload.m_entryDst, payload.m_markerEntryRetreived );
        msg.returnToSender();
    }


    /// See Cpl::Persistent::GetByBufferIndexRequest
    void request( GetByBufferIndexMsg& msg )
    {
        GetByBufferIndexRequest::Payload& payload = msg.getPayload();
        payload.m_success                         = m_record.getByBufferIndex( payload.m_index, payload.m_entryDst, payload.m_markerEntryRetreived );
        msg.returnToSender();
    }

    /// See Cpl::Persistent::IndexEntryReader
    size_t getMaxIndex() const noexcept
    {
        return m_record.getMaxIndex();  // Note: NO Critical section is required since this is a 'constant' value
    }

    /// See Cpl::Persistent::ClearAllEntriesRequest
    void request( ClearAllEntriesMsg& msg )
    {
        ClearAllEntriesRequest::Payload& payload = msg.getPayload();
        payload.m_success                        = m_record.clearAllEntries();
        msg.returnToSender();
    }

protected:
    /// Indexed Entry Record that handles the actual work to read/write the data
    IndexedEntryRecord&                     m_record;

    /// Observer for change notification (to the RingBuffer)
    Cpl::Dm::SubscriberComposer<IndexedEntryServer, Cpl::Dm::Mp::Uint32>  m_observerElementCount;

    /// the 'add' buffer
    Cpl::Container::RingBufferMP<ENTRY>&    m_addBuffer;

    /// Track my open state
    bool                                    m_opened;
};



};      // end namespaces
};
#endif  // end header latch