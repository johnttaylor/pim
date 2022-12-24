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

#include "Catch/catch.hpp"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Text/FString.h"
#include "Storm/Dm/MpSetpoints.h"
#include "Cpl/Math/real.h"

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
            REQUIRE( memcmp( &elem, &m_elemValue, sizeof(ELEMTYPE) ) == 0 );

            m_masterThread.signal();
        }
    }
};


class ViewerSetpoint: public Cpl::Itc::CloseSync
{
public:
    ///
    Cpl::System::Thread&                                                m_masterThread;
    ///
    Cpl::Dm::SubscriberComposer<ViewerSetpoint, Storm::Dm::MpSetpoints> m_observerMp1;
    ///
    Storm::Dm::MpSetpoints&                                             m_mp;
    ///
    Storm::Dm::MpSetpoints::Data                                        m_elemValue;


    /// Constructor
    ViewerSetpoint( Cpl::Dm::MailboxServer& myMbox, Cpl::System::Thread& masterThread, Storm::Dm::MpSetpoints& mpToMonitor, Storm::Dm::MpSetpoints::Data  elemValue )
        : Cpl::Itc::CloseSync( myMbox )
        , m_masterThread( masterThread )
        , m_observerMp1( myMbox, *this, &ViewerSetpoint::mp1_changed )
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
    void mp1_changed( Storm::Dm::MpSetpoints& modelPointThatChanged, Cpl::Dm::SubscriberApi& clientObserver ) noexcept
    {
        if ( modelPointThatChanged.isNotValidAndSync( clientObserver ) == false )
        {
            Storm::Dm::MpSetpoints::Data elem;
            REQUIRE( modelPointThatChanged.readAndSync( elem, clientObserver ) );
            REQUIRE( Cpl::Math::areFloatsEqual( m_elemValue.coolSetpt, elem.coolSetpt ) );
            REQUIRE( Cpl::Math::areFloatsEqual( m_elemValue.heatSetpt, elem.heatSetpt ) );

            m_masterThread.signal();
        }
    }
};
#endif // end header latch