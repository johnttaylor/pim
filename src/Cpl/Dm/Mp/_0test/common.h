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

#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Catch/catch.hpp"


/// Macro to help shutdown the observer thread
#define WAIT_FOR_THREAD_TO_STOP(t)      for ( int i=0; i < 200; i++ ) \
                                        { \
                                            Cpl::System::Api::sleep( 50 ); \
                                            if ( t->isRunning() == false ) \
                                            { \
                                                break; \
                                            } \
                                        }


template< class MPTYPE, class ELEMTYPE>
class Viewer : public Cpl::Itc::CloseSync
{
public:
    ///
    Cpl::System::Thread&                            m_masterThread;
    ///
    Cpl::Dm::SubscriberComposer<Viewer, MPTYPE>     m_observerMp1;
    ///
    MPTYPE&                                         m_mp;
    ///
    ELEMTYPE                                        m_elemValue;

    /// Constructor
    Viewer( Cpl::Dm::MailboxServer& myMbox, Cpl::System::Thread& masterThread, MPTYPE& mpToMonitor, ELEMTYPE elemValue )
        : Cpl::Itc::CloseSync( myMbox )
        , m_masterThread( masterThread )
        , m_observerMp1( myMbox, *this, &Viewer<MPTYPE,ELEMTYPE>::mp1_changed )
        , m_mp( mpToMonitor )
        , m_elemValue( elemValue )
    {
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        m_mp.attach( m_observerMp1 );
        msg.returnToSender();
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        m_mp.detach( m_observerMp1 );
        msg.returnToSender();
    }


public:
    void mp1_changed( MPTYPE& modelPointThatChanged, Cpl::Dm::SubscriberApi& clientObserver ) noexcept
    {
        if ( modelPointThatChanged.isNotValidAndSync( clientObserver ) == false )
        {
            ELEMTYPE elem;
            REQUIRE( modelPointThatChanged.readAndSync( elem, clientObserver ) );
            REQUIRE( elem == m_elemValue );

            m_masterThread.signal();
        }
    }
};


template< class MPTYPE, int N>
class ViewerUint8Array : public Cpl::Itc::CloseSync
{
public:
    ///
    Cpl::System::Thread&                                m_masterThread;
    ///
    Cpl::Dm::SubscriberComposer<ViewerUint8Array, MPTYPE>    m_observerMp1;
    ///
    MPTYPE&                                             m_mp;
    ///
    uint8_t                                             m_val[N];

    /// Constructor
    ViewerUint8Array( Cpl::Dm::MailboxServer& myMbox, Cpl::System::Thread& masterThread, MPTYPE& mpToMonitor, uint8_t* array )
        : Cpl::Itc::CloseSync( myMbox )
        , m_masterThread( masterThread )
        , m_observerMp1( myMbox, *this, &ViewerUint8Array<MPTYPE,N>::mp1_changed )
        , m_mp( mpToMonitor )
    {
        memcpy( m_val, array, N );
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        m_mp.attach( m_observerMp1 );
        msg.returnToSender();
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        m_mp.detach( m_observerMp1 );
        msg.returnToSender();
    }


public:
    void mp1_changed( MPTYPE& modelPointThatChanged, Cpl::Dm::SubscriberApi& clientObserver ) noexcept
    {
        if ( modelPointThatChanged.isNotValidAndSync( clientObserver ) == false )
        {
            uint8_t val[N] ={ 0, };
            REQUIRE( modelPointThatChanged.readAndSync( val, N, clientObserver ) );
            REQUIRE( memcmp( val, m_val, N ) == 0 );

            m_masterThread.signal();
        }
    }
};

#endif // end header latch