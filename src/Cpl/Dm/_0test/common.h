#ifndef common_h_
#define common_h_
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

#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/RmwComposer.h"
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
    int8_t                              m_lastValidState;
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
        , m_lastValidState( OPTION_CPL_DM_MODEL_POINT_STATE_INVALID )
        , m_done( false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "VIEWER(%p). mp1=%s, endVal=%lu", this, mp1.getName(), mpEndValue1 ) );
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "SUBSCRIBING (%p) for Change notification. current value =%u", this, m_mpNotificationCount1 ) );

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

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "VIEWER(%p): Closing... ", this ) );

        // Un-subscribe to my model point
        m_mp1.detach( m_observerMp1 );
        m_opened = false;
        msg.returnToSender();
    }


public:
    void mp1_changed( Mp::Uint32& modelPointThatChanged ) noexcept
    {
        CPL_SYSTEM_TRACE_ALLOCATE( uint32_t, prevValue, m_lastValue );
        CPL_SYSTEM_TRACE_ALLOCATE( int8_t,   prevState, m_lastValidState );
        uint16_t prevSeqNum = m_lastSeqNumber;

        m_mpNotificationCount1++;
        uint16_t seqNum;
        m_lastValidState = modelPointThatChanged.read( m_lastValue, &seqNum );
        m_lastSeqNumber  = m_observerMp1.getSequenceNumber_();

        if ( m_pendingOpenMsgPtr != 0 && m_mpNotificationCount1 == 1 )
        {
            m_pendingOpenMsgPtr->returnToSender();
            m_opened            = true;
            m_pendingOpenMsgPtr = 0;
        }

        if ( ModelPoint::IS_VALID( m_lastValidState ) && m_lastValue >= m_mpEndValue )
        {
            if ( m_done )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ( "Viewer::mp1_changed(%p): Received Change notification after signaling the master thread, may or may not be an error. Prev: value=%lu, state=%d, seqNum=%u.  Rcvd: value=%lu, state=%d, seqNum=%u.  read_seq_num=%u, notifyCount=%d", this, prevValue, prevState, prevSeqNum, m_lastValue, m_lastValidState, m_lastSeqNumber, seqNum, m_mpNotificationCount1 ) );
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ( "Viewer::mp1_changed(%p): Signaling master thread", this ) );
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "WRITER(%p). mp1=%s, endVal=%lu, interval=%lu", this, mp1.getName(), endValue, intervalMsec ) );
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            FAIL( "OPENING Writer more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "WRITER(%p): Starting interval timer (%lu)", this, m_intervalMsec ) );
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

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "WRITER(%p): Closing... ", this ) );

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
                CPL_SYSTEM_TRACE_MSG( SECT_, ( "Writer::timerExpired(%p): Signaling master thread", this ) );
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
class Rmw : public Cpl::Itc::CloseSync
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
    Cpl::System::TimerComposer<Rmw>     m_timer;
    ///
    RmwComposer<Rmw, uint32_t>          m_rmwHandler;

    /// Constructor
    Rmw( MailboxServer& myMbox, Cpl::System::Thread& masterThread, Mp::Uint32& mp1, unsigned long intervalMsec, uint32_t startValue, uint32_t endValue, uint32_t stepSize )
        :Cpl::Itc::CloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_mp1( mp1 )
        , m_intervalMsec( intervalMsec )
        , m_writeCount( 0 )
        , m_currentValue( startValue )
        , m_endValue( endValue )
        , m_stepSize( stepSize )
        , m_timer( myMbox, *this, &Rmw::timerExpired )
        , m_rmwHandler( *this, &Rmw::readModifyWriteCallback )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "RMW(%p). mp1=%s, endVal=%lu, interval=%lu", this, mp1.getName(), endValue, intervalMsec ) );
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        if ( m_opened )
        {
            FAIL( "OPENING RMW more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "RMW(%p): Starting interval timer (%lu)", this, m_intervalMsec ) );
        m_opened = true;
        m_timer.start( m_intervalMsec );
        msg.returnToSender();
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING RMW more than ONCE" );
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "RMW(%p): Closing... ", this ) );

        // Stop my writ timer
        m_opened = false;
        m_timer.stop();
        msg.returnToSender();
    }


public:
    ///
    ModelPoint::RmwCallbackResult_T readModifyWriteCallback( uint32_t& data, int8_t validState )
    {
        if ( data >= m_endValue )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "RMW::readModifyWriteCallback(%p): Signaling master thread", this ) );
            m_masterThread.signal();
        }
        else
        {
            data += m_stepSize;
            m_timer.start( m_intervalMsec );
        }

        m_currentValue = data;
        return ModelPoint::eCHANGED;
    }

    ///
    void timerExpired( void )
    {
        if ( m_opened )
        {
            m_writeCount++;
            m_mp1.readModifyWrite( m_rmwHandler );
        }
        else
        {
            FAIL( "RMW: SHOULDED HAPPEN - The interval timer expired before the RMW was opened" );
        }
    }
};

class RmwUint32 : public Mp::Uint32::Client
{
public:
    ///
    int m_callbackCount;
    ///
    ModelPoint::RmwCallbackResult_T m_returnResult;
    ///
    uint32_t                        m_incValue;
public:
    ///
    RmwUint32() :m_callbackCount( 0 ), m_returnResult( ModelPoint::eNO_CHANGE ), m_incValue( 0 ) {}
    ///
    ModelPoint::RmwCallbackResult_T callback( uint32_t& data, int8_t validState ) noexcept
    {
        m_callbackCount++;
        if ( m_returnResult != ModelPoint::eNO_CHANGE )
        {
            data += m_incValue;
        }
        return m_returnResult;
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
    uint32_t                            m_mpNotificationCount;
    ///
    uint32_t                            m_mpEndValue;
    ///
    SubscriberComposer<GenericViewer, ModelPoint> m_observerMp1;
    ///
    SubscriberComposer<GenericViewer, ModelPoint> m_observerMp2;
    ///
    SubscriberComposer<GenericViewer, ModelPoint> m_observerMp3;
    ///
    uint16_t                            m_lastSeqNumber;
    ///
    uint32_t                            m_lastValue;
    ///
    int8_t                              m_lastValidState;
    ///
    bool                                m_done;

    /// Constructor
    GenericViewer( MailboxServer& myMbox, Cpl::System::Thread& masterThread, ModelPoint& mp1, ModelPoint& mp2, ModelPoint& mp3, uint32_t mpEndValue3 )
        :Cpl::Itc::CloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_pendingOpenMsgPtr( 0 )
        , m_mp1( mp1 )
        , m_mp2( mp2 )
        , m_mp3( mp3 )
        , m_mpNotificationCount( 0 )
        , m_mpEndValue( mpEndValue3 )
        , m_observerMp1( myMbox, *this, &GenericViewer::mpChanged )
        , m_observerMp2( myMbox, *this, &GenericViewer::mpChanged )
        , m_observerMp3( myMbox, *this, &GenericViewer::mpChanged )
        , m_lastSeqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
        , m_lastValidState( OPTION_CPL_DM_MODEL_POINT_STATE_INVALID )
        , m_done( false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "GENERIC VIEWER(%p). mp1=%s, endVal3=%lu", this, mp1.getName(), mpEndValue3 ) );
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "GENERIC SUBSCRIBING (%p) for Change notification. current value =%u", this, m_mpNotificationCount ) );

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

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "GENERIC VIEWER(%p): Closing... ", this ) );

        // Un-subscribe to my model point
        m_mp1.genericDetach( m_observerMp1 );
        m_mp2.genericDetach( m_observerMp2 );
        m_mp3.genericDetach( m_observerMp3 );
        m_opened = false;
        msg.returnToSender();
    }


public:
    void mpChanged( ModelPoint& modelPointThatChanged ) noexcept
    {
        CPL_SYSTEM_TRACE_ALLOCATE( uint32_t, prevValue,  m_lastValue  );
        CPL_SYSTEM_TRACE_ALLOCATE( int8_t,   prevState,  m_lastValidState );
        CPL_SYSTEM_TRACE_ALLOCATE( uint16_t, prevSeqNum, m_lastSeqNumber );

        m_mpNotificationCount++;
        m_lastValidState = modelPointThatChanged.getValidState();
        m_lastSeqNumber  = m_observerMp1.getSequenceNumber_();

        if ( m_pendingOpenMsgPtr != 0 && m_mpNotificationCount == 3 )
        {
            m_pendingOpenMsgPtr->returnToSender();
            m_opened            = true;
            m_pendingOpenMsgPtr = 0;
        }

        if ( ModelPoint::IS_VALID( m_lastValidState ) && m_mpNotificationCount >= m_mpEndValue )
        {
            if ( m_done )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ( "Generic Viewer::mpChanged(%p): Received Change notification after signaling the master thread, may or may not be an error. Prev: value=%lu, state=%d, seqNum=%u.  Rcvd: state=%d, seqNum=%u.  read_seq_num=%u, notifyCount=%d", this, prevValue, prevState, prevSeqNum, m_lastValidState, m_lastSeqNumber, m_mpNotificationCount ) );
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ( "GenericViewer::mp1Changed(%p): Signaling master thread", this ) );
                m_mp1.genericDetach( m_observerMp1 );
                m_mp2.genericDetach( m_observerMp2 );
                m_mp3.genericDetach( m_observerMp3 );
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};


/////////////////////////////////////////////////////////////////
class MyStaticInfo : public StaticInfo
{
public:
    /// Constructor
    MyStaticInfo(const char* name, const char* units, const char* description)
        : StaticInfo(name)
        , m_units(units)
        , m_description(description)
    {
    }

public:
    bool hasJSONFormatter() const noexcept { return true; }
    void toJSON(JsonObject& dstObject) const noexcept
    {
        dstObject["units"] = m_units;
        dstObject["desc"] = m_description;
    }

public:
    const char* m_units;
    const char* m_description;
};

#endif