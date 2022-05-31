#ifndef Common_h_
#define Common_h_
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

#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Timer.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Itc/AtomicCallback.h"
#include "Cpl/Itc/AtomicRequest.h"
#include "Cpl/Itc/AtomicSync.h"
#include "Cpl/Container/DList.h"
#include "Cpl/Itc/_0test/WriteSync.h"
#include "Cpl/Itc/_0test/ViewRequest.h"
#include "Cpl/Itc/_0test/ReadModifyWrite.h"


#define SECT_           "_0test"
#ifndef NUM_SEQ_
#define NUM_SEQ_        3
#endif
#define NUM_WRITES_     5
#define ATOMIC_MODIFY_  2

#define TIMER_DELAY     100

////////////////////////////////////////////////////////////////////////////////
class Master : public Cpl::System::Runnable,
    public Cpl::Itc::AtomicCallback<ReadModifyWrite>
{
public:
    ///
    int                                     m_numSequences;
    ///
    int                                     m_numWrites;
    ///
    Cpl::Itc::CloseSync&                    m_viewer;
    ///
    Cpl::Itc::AtomicApi<ReadModifyWrite>&   m_readModifyWriteApi;
    ///
    WriteApi&                               m_writeApi;
    ///
    Cpl::System::Signable&                  m_waiter;
    ///
    bool                                    m_simulation;


public:
    ///
    Master( int numSeqs, int numWrites, Cpl::Itc::CloseSync& viewer, Cpl::Itc::AtomicApi<ReadModifyWrite>& readModifyWriteApi, WriteApi& writeApi, Cpl::System::Signable& waiter, bool simulation=false )
        :m_numSequences( numSeqs ),
        m_numWrites( numWrites ),
        m_viewer( viewer ),
        m_readModifyWriteApi( readModifyWriteApi ),
        m_writeApi( writeApi ),
        m_waiter( waiter ),
        m_simulation( simulation )
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, Cpl::System::Thread::getCurrent().getName() );

        int i;
        for ( i=0; i < m_numSequences; i++ )
        {

            CPL_SYSTEM_TRACE_MSG( SECT_, ("----- Starting Sequence#: %d....", i + 1) );
            m_viewer.open();

            int j;
            for ( j=0; j < m_numWrites; j++ )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("** Write Operation#: %d", j + 1) );
                m_writeApi.write( j );
                Cpl::System::Api::sleep( 50 ); // Delay to allow other threads to run.
            }

            // Test the Atomic Request stuff
            CPL_SYSTEM_TRACE_MSG( SECT_, ("-----   Performing Atomic read-modify-write operation...") );
            m_readModifyWriteApi.executeAtomic( *this );
            Cpl::System::Api::sleep( 50 ); // Delay to allow other threads to run.

            m_viewer.close();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("----- End Sequence#: %d.", i + 1) );

            // Handshake with the supervising thread to validate sequence results
            m_waiter.signal();
            Cpl::System::Thread::wait();
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("----- Signal main thread that I am done") );
        m_waiter.signal();
    }

public:
    ///  Atomic Call back
    void execute( ReadModifyWrite& api )
    {
        CPL_SYSTEM_TRACE_FUNC( SECT_ );
        int t  = api.getValue();        // Read
        t     *= ATOMIC_MODIFY_;        // Modify
        api.writeValue( t );            // Write
    }

};

class MyMailboxServer : public Cpl::Itc::MailboxServer
{
public:
    ///
    volatile int m_sigCount;
    ///
    volatile int m_sigCountUnexpected;
    ///
    uint32_t     m_expectedEvents;

    /// Constructor
    MyMailboxServer( uint32_t expectedEvents ):m_sigCount( 0 ), m_sigCountUnexpected( 0 ), m_expectedEvents( expectedEvents ) {}

public:
    ///
    void processEventFlag( uint8_t eventNumber ) noexcept
    {
        if ( (1 << eventNumber) & m_expectedEvents )
        {
            m_sigCount++;
        }
        else
        {
            m_sigCountUnexpected++;
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("MyMailboxServer::processEventFlag(). eventNumber=%u, count=%d, unexpected count=%d", eventNumber, m_sigCount, m_sigCountUnexpected) );
    }
};



class Model : public WriteSync,
    public ViewRequest,
    public Cpl::Itc::AtomicSync<ReadModifyWrite>
{
public:
    ///
    volatile int m_value;
    ///
    Cpl::Container::DList<ViewRequest::AttachMsg> m_subscribers;


public:
    ///
    Model( Cpl::Itc::PostApi& myMailbox, int value=0 )
        :WriteSync( myMailbox ),
        Cpl::Itc::AtomicSync<ReadModifyWrite>( myMailbox ),
        m_value( 0 )
    {
    }


public:
    ///
    void request( AttachMsg& msg )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("request( AttachMsg& %p ): client value=%d, model value=%d", &msg, msg.getPayload().m_value, m_value) );

        // Notify the client if his value is different from the model (also update the client's value to match mine on notification)
        if ( msg.getPayload().m_value != m_value )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("immediate CHANGE notification (%p)", &msg) );
            msg.getPayload().m_value = m_value;
            msg.returnToSender();
        }

        // SAME value -->hold notification message until there is change
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Queued change notification (%p)", &msg) );
            m_subscribers.put( msg );
        }
    }


    ///
    void request( DetachMsg& msg )
    {
        // Remove from my subscriber list
        AttachMsg& attachMsg = msg.getPayload().m_msgToDetach;
        if ( m_subscribers.remove( attachMsg ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("CANCELED change notification (%p)", &attachMsg) );
            attachMsg.returnToSender();    // In subscriber list -->return the notification message
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("attempted to CANCEL change notification (%p) -->NOT pending -->nothing done", &attachMsg) );
        }

        // Return the cancel message last, this ensure that when the client receives the response to the Detach Msg, he has also been returned the original AttachMsg
        msg.returnToSender();
    }

public:
    /// Internal helper method
    void processChangeNotificaitons()
    {
        // Notify my subscribers
        Cpl::Container::DList<AttachMsg> tempList;
        AttachMsg*                       subscriberPtr = m_subscribers.get();
        while ( subscriberPtr )
        {
            // Check if new value is different from the subscriber's
            if ( subscriberPtr->getPayload().m_value != m_value )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("send CHANGE notification (%p), new_value=%d", subscriberPtr, m_value) );
                subscriberPtr->getPayload().m_value = m_value;
                subscriberPtr->returnToSender();
            }

            // SAME value -->hold notification message until there is change
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Write request -->no update/notification for client: (%p)", subscriberPtr) );
                tempList.put( *subscriberPtr );
            }

            // Get the next subscriber
            subscriberPtr = m_subscribers.get();
        }

        // Restore my subscriber list to any subscriber(s) that did NOT need change notification
        tempList.move( m_subscribers );
    }


public:
    ///
    virtual void request( WriteMsg& msg )
    {
        // Update the model
        m_value = msg.getPayload().m_value;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("request( WriteMsg& %p ): new value=%d", &msg, m_value) );
        processChangeNotificaitons();

        // Return the write message
        msg.returnToSender();
    }


public: // AtomicRequest<ReadModifyWrite> methods

    /// Read the data
    int getValue( void ) noexcept { CPL_SYSTEM_TRACE_FUNC( SECT_ ); return m_value; }

    /// Write the data
    void writeValue( int newValue ) noexcept { CPL_SYSTEM_TRACE_FUNC( SECT_ ); m_value = newValue; }

    /// Intercept base atomic-request call
    void request( ReqMsg& msg )
    {
        AtomicRequest<ReadModifyWrite>::request( msg );
        processChangeNotificaitons();
    }

};


class Viewer : public Cpl::Itc::CloseSync,
    public ViewResponseApi,
    public Cpl::System::Timer
{
public:
    ///
    volatile bool                       m_opened;
    ///
    Cpl::Itc::CloseRequest::CloseMsg*   m_pendingCloseMsgPtr;
    ///
    ViewRequest::AttachPayload          m_attachPayload;
    ///
    ViewResponseApi::AttachMsg          m_attachRspMsg;
    ///
    ViewRequest::DetachPayload          m_detachPayload;
    ///
    ViewResponseApi::DetachMsg          m_detachRspMsg;
    ///
    ViewRequest::SAP&                   m_viewReqSAP;
    ///
    volatile bool                       m_ownAttachMsg;
    ///
    volatile bool                       m_ownDetachMsg;
    ///
    unsigned long                       m_timerExpiredCount;

    /// Constructor
    Viewer( int initialValue, Cpl::Itc::MailboxServer& viewersMbox, ViewRequest::SAP& viewSAP )
        :Cpl::Itc::CloseSync( viewersMbox ),
        Cpl::System::Timer( viewersMbox ),
        m_opened( false ),
        m_pendingCloseMsgPtr( 0 ),
        m_attachPayload( initialValue ),
        m_attachRspMsg( *this, viewersMbox, viewSAP, m_attachPayload ),
        m_detachPayload( m_attachRspMsg.getRequestMsg() ),
        m_detachRspMsg( *this, viewersMbox, viewSAP, m_detachPayload ),
        m_viewReqSAP( viewSAP ),
        m_ownAttachMsg( true ),
        m_ownDetachMsg( true ),
        m_timerExpiredCount( 0 )
    {
    }


protected:
    void expired( void ) noexcept
    {
        if ( m_opened )
        {
            m_timerExpiredCount++;
            start( TIMER_DELAY );
        }
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            Cpl::System::FatalError::log( "OPENING Viewer more than ONCE" );
        }

        // Start my timer
        start( TIMER_DELAY );

        CPL_SYSTEM_TRACE_MSG( SECT_, ("SUBSCRIBING for Change notification. current value=%d", m_attachRspMsg.getRequestMsg().getPayload().m_value) );
        m_opened       = true;
        m_ownAttachMsg = false;
        m_viewReqSAP.post( m_attachRspMsg.getRequestMsg() );
        msg.returnToSender();
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        if ( !m_opened )
        {
            Cpl::System::FatalError::log( "CLOSING Viewer more than ONCE" );
        }

        m_ownDetachMsg       = false;
        m_pendingCloseMsgPtr = &msg;
        m_viewReqSAP.post( m_detachRspMsg.getRequestMsg() );
    }

public:
    ///
    void response( AttachMsg& msg )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("RECIEVED Change notification. new value=%d", msg.getPayload().m_value) );

        // Immediately re-subscribe (if NOT closing down)
        if ( !m_pendingCloseMsgPtr )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("SUBSCRIBING for Change notification. current value=%d", msg.getPayload().m_value) );
            m_viewReqSAP.post( msg.getRequestMsg() );
        }

        m_ownAttachMsg = true;
    }


    /// 
    void response( DetachMsg& msg )
    {
        CPL_SYSTEM_TRACE_FUNC( SECT_ );

        if ( !m_pendingCloseMsgPtr )
        {
            Cpl::System::FatalError::log( "SUBSCRIPTION to Viewer CANCELLED -->but not in the 'Closing' state" );
        }

        m_pendingCloseMsgPtr->returnToSender();
        m_pendingCloseMsgPtr = 0;
        m_opened             = false;
        m_ownDetachMsg       = true;
        stop();
    }


};



#endif // end header latch
