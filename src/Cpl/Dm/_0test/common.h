#ifndef common_h_
#define common_h_
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

#include "Catch/catch.hpp"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Timer.h"

/// 
using namespace Cpl::Dm;

#define SECT_   "_0test"

#define TRACE

/////////////////////////////////////////////////////////////////
class Viewer : public Cpl::Itc::CloseSync
{
public:
    ///
    volatile bool                       m_opened;
    ///
    Cpl::System::Thread&                m_masterThread;
    ///
    Cpl::Itc::OpenRequest::OpenMsg*     m_pendingOpenMsgPtr;
    ///
    Mp::Uint32&                         m_mp1;
    ///
    uint32_t                            m_mpNotificationCount1;
    ///
    uint32_t                            m_mpEndValue;
    ///
    SubscriberComposer<Viewer, Mp::Uint32>   m_observerMp1;
    ///
    uint16_t                            m_lastSeqNumber;
    ///
    uint32_t                            m_lastValue;
    ///
    bool                                m_lastValid;
    ///
    bool                                m_done;

    /// Constructor
    Viewer( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Uint32& mp1, uint32_t mpEndValue1 )
        :Cpl::Itc::CloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_pendingOpenMsgPtr( 0 )
        , m_mp1( mp1 )
        , m_mpNotificationCount1( 0 )
        , m_mpEndValue( mpEndValue1 )
        , m_observerMp1( myMbox, *this, &Viewer::mp1_changed )
        , m_lastSeqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
        , m_lastValue( 0 )
        , m_lastValid( false )
        , m_done( false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("VIEWER(%p). mp1=%s, endVal=%lu", this, mp1.getName(), mpEndValue1) );
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            FAIL( "OPENING Viewer more than ONCE" );
        }

        m_pendingOpenMsgPtr    = &msg;
        m_mpNotificationCount1 = 0;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("SUBSCRIBING (%p) for Change notification. current value =%u", this, m_mpNotificationCount1) );

        // Subscribe to my model point
        m_mp1.attach( m_observerMp1 );

        // Note: The open message will be returned once all the model point receives its initial callback
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING Viewer more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("VIEWER(%p): Closing... ", this) );

        // Un-subscribe to my model point
        m_mp1.detach( m_observerMp1 );
        m_opened = false;
        msg.returnToSender();
    }


public:
    void mp1_changed( Mp::Uint32& modelPointThatChanged, SubscriberApi& clientObsever ) noexcept
    {
        CPL_SYSTEM_TRACE_ALLOCATE( uint32_t, prevValue, m_lastValue );
        CPL_SYSTEM_TRACE_ALLOCATE( int8_t, prevState, m_lastValid );
        uint16_t prevSeqNum = m_lastSeqNumber;

        m_mpNotificationCount1++;
        m_lastValid     = modelPointThatChanged.readAndSync( m_lastValue, clientObsever );
        m_lastSeqNumber = m_observerMp1.getSequenceNumber_();
        REQUIRE( m_observerMp1.getSequenceNumber_() == modelPointThatChanged.getSequenceNumber() );

        //CPL_SYSTEM_TRACE_MSG( SECT_, ("VIEWER(%p): mp changed (%s), notify count=%d, valid=%d, value=%d",
        //                               this,
        //                               modelPointThatChanged.getName(),
        //                               m_mpNotificationCount1,
        //                               m_lastValid,
        //                               m_lastValue) );

        if ( m_pendingOpenMsgPtr != 0 && m_mpNotificationCount1 == 1 )
        {
            m_pendingOpenMsgPtr->returnToSender();
            m_opened               = true;
            m_pendingOpenMsgPtr    = 0;
            m_mpNotificationCount1 = 0;
        }

        if ( m_lastValid && m_lastValue >= m_mpEndValue )
        {
            if ( m_done )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer::mp1_changed(%p): Received Change notification after signaling the master thread, may or may not be an error. Prev: value=%lu, state=%d, seqNum=%u.  Rcvd: value=%lu, state=%d, seqNum=%u.  read_seq_num=%u, notifyCount=%d", this, prevValue, prevState, prevSeqNum, m_lastValue, m_lastValid, m_lastSeqNumber, m_lastSeqNumber, m_mpNotificationCount1) );
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer::mp1_changed(%p, %s): Signaling master thread", this, m_mp1.getName() ) );
                m_mp1.detach( m_observerMp1 );
                m_masterThread.signal();
                m_done = true;
            }
        }

        REQUIRE( prevSeqNum != m_lastSeqNumber );
    }

};

/////////////////////////////////////////////////////////////////
class Writer : public Cpl::Itc::CloseSync
{
public:
    ///
    volatile bool                       m_opened;
    ///
    Cpl::System::Thread&                m_masterThread;
    ///
    Mp::Uint32&                         m_mp1;
    ///
    unsigned long                       m_intervalMsec;
    ///
    uint32_t                            m_writeCount;
    ///
    uint32_t                            m_currentValue;
    ///
    uint32_t                            m_endValue;
    ///
    uint32_t                            m_stepSize;
    ///
    Cpl::System::TimerComposer<Writer>  m_timer;

    /// Constructor
    Writer( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Uint32& mp1, unsigned long intervalMsec, uint32_t startValue, uint32_t endValue, uint32_t stepSize )
        :Cpl::Itc::CloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_mp1( mp1 )
        , m_intervalMsec( intervalMsec )
        , m_writeCount( 0 )
        , m_currentValue( startValue )
        , m_endValue( endValue )
        , m_stepSize( stepSize )
        , m_timer( myMbox, *this, &Writer::timerExpired )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("WRITER(%p). mp1=%s, endVal=%lu, interval=%lu", this, mp1.getName(), endValue, intervalMsec) );
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            FAIL( "OPENING Writer more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("WRITER(%p): Starting interval timer (%lu)", this, m_intervalMsec) );
        m_opened = true;
        m_timer.start( m_intervalMsec );
        msg.returnToSender();
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING Writer more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("WRITER(%p): Closing... ", this) );

        // Stop my writ timer
        m_opened = false;
        m_timer.stop();
        msg.returnToSender();
    }


public:
    ///
    void timerExpired( void )
    {
        if ( m_opened )
        {
            m_writeCount++;
            m_mp1.write( m_currentValue );

            if ( m_currentValue >= m_endValue )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Writer::timerExpired(%p): Signaling master thread", this) );
                m_masterThread.signal();
            }
            else
            {
                m_currentValue += m_stepSize;
                m_timer.start( m_intervalMsec );
            }
        }
        else
        {
            FAIL( "WRITER: SHOULDED HAPPEN - The interval timer expired before the Writer was opened" );
        }
    }
};


/////////////////////////////////////////////////////////////////
class GenericViewer : public Cpl::Itc::CloseSync
{
public:
    ///
    volatile bool                       m_opened;
    ///
    Cpl::System::Thread&                m_masterThread;
    ///
    Cpl::Itc::OpenRequest::OpenMsg*     m_pendingOpenMsgPtr;
    ///
    ModelPoint&                         m_mp1;
    ///
    ModelPoint&                         m_mp2;
    ///
    ModelPoint&                         m_mp3;
    ///
    unsigned                            m_maxNotifyCounts;
    ///
    uint32_t                            m_mpNotificationCount;
    ///
    SubscriberComposer<GenericViewer, ModelPoint> m_observerMp1;
    ///
    SubscriberComposer<GenericViewer, ModelPoint> m_observerMp2;
    ///
    SubscriberComposer<GenericViewer, ModelPoint> m_observerMp3;
    ///
    uint16_t                            m_lastSeqNumber;
    ///
    bool                                m_lastValid;
    ///
    bool                                m_done;

    /// Constructor
    GenericViewer( MailboxServer& myMbox, Cpl::System::Thread& masterThread, ModelPoint& mp1, ModelPoint& mp2, ModelPoint& mp3, unsigned maxNotifyCounts )
        :Cpl::Itc::CloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_pendingOpenMsgPtr( 0 )
        , m_mp1( mp1 )
        , m_mp2( mp2 )
        , m_mp3( mp3 )
        , m_maxNotifyCounts( maxNotifyCounts )
        , m_mpNotificationCount( 0 )
        , m_observerMp1( myMbox, *this, &GenericViewer::mpChanged )
        , m_observerMp2( myMbox, *this, &GenericViewer::mpChanged )
        , m_observerMp3( myMbox, *this, &GenericViewer::mpChanged )
        , m_lastSeqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
        , m_lastValid( false )
        , m_done( false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("GENERIC VIEWER(%p). mp1=%s", this, mp1.getName()) );
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            FAIL( "OPENING Generic Viewer more than ONCE" );
        }

        m_pendingOpenMsgPtr   = &msg;
        m_mpNotificationCount = 0;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("GENERIC SUBSCRIBING (%p) for Change notification. current value =%u", this, m_mpNotificationCount) );

        // Subscribe to my model point
        m_mp1.genericAttach( m_observerMp1 );
        m_mp2.genericAttach( m_observerMp2 );
        m_mp3.genericAttach( m_observerMp3 );

        // Note: The open message will be returned once all the model point receives its initial callback
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING GENERIC Viewer more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ("GENERIC VIEWER(%p): Closing... ", this) );

        // Un-subscribe to my model point
        m_mp1.genericDetach( m_observerMp1 );
        m_mp2.genericDetach( m_observerMp2 );
        m_mp3.genericDetach( m_observerMp3 );
        m_opened = false;
        msg.returnToSender();
    }


public:
    void mpChanged( ModelPoint& modelPointThatChanged, SubscriberApi& clientObserver  ) noexcept
    {
        m_mpNotificationCount++;
        m_lastValid     = !modelPointThatChanged.isNotValid();
        m_lastSeqNumber = m_observerMp1.getSequenceNumber_();

        if ( m_pendingOpenMsgPtr != 0 && m_mpNotificationCount == 3 )
        {
            m_pendingOpenMsgPtr->returnToSender();
            m_opened              = true;
            m_pendingOpenMsgPtr   = 0;
            m_mpNotificationCount = 0;
        }

        //CPL_SYSTEM_TRACE_MSG( SECT_, ("Generic VIEWER(%p): mp changed (%s), notify count=%d (of %d), valid=%d",
        //                               this,
        //                               modelPointThatChanged.getName(),
        //                               m_mpNotificationCount,
        //                               m_maxNotifyCounts,
        //                               m_lastValid) );


        if ( m_lastValid && m_mpNotificationCount >= m_maxNotifyCounts / 2 )   // Assume at least half the change notifications
        {
            if ( m_mpNotificationCount > m_maxNotifyCounts )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Generic View: Received TOO many change notification (%d, max=%d)", m_mpNotificationCount, m_maxNotifyCounts) );
                FAIL( "Generic View: Received TOO many change notification" );
            }

            if ( m_done )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Generic Viewer::mpChanged(%p): Received Change notification after signaling the master thread, may or may not be an error.", this) );
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("GenericViewer::mp1Changed(%p): Signaling master thread", this) );
                m_mp1.genericDetach( m_observerMp1 );
                m_mp2.genericDetach( m_observerMp2 );
                m_mp3.genericDetach( m_observerMp3 );
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

#endif