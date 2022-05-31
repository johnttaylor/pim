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
#include "Cpl/Itc/CloseSync.h"


/// Macro to help shutdown the observer thread
#define WAIT_FOR_THREAD_TO_STOP(t)      for ( int i=0; i < 200; i++ ) \
                                        { \
                                            Cpl::System::Api::sleep( 50 ); \
                                            if ( t->isRunning() == false ) \
                                            { \
                                                break; \
                                            } \
                                        }


template< class MPTYPE>
class Viewer : public Cpl::Itc::CloseSync
{
public:
    ///
    Cpl::System::Thread&                            m_masterThread;
    ///
    Cpl::Dm::SubscriberComposer<Viewer, MPTYPE>     m_observerMp1;
    ///
    MPTYPE&                                         m_mp;


    /// Constructor
    Viewer( Cpl::Dm::MailboxServer& myMbox, Cpl::System::Thread& masterThread, MPTYPE& mpToMonitor )
        : Cpl::Itc::CloseSync( myMbox )
        , m_masterThread( masterThread )
        , m_observerMp1( myMbox, *this, &Viewer<MPTYPE>::mp1_changed )
        , m_mp( mpToMonitor )
    {
    }

public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        m_mp.attach( m_observerMp1, m_mp.getSequenceNumber() );
        msg.returnToSender();
    }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
    {
        m_mp.detach( m_observerMp1 );
        msg.returnToSender();
    }


public:
    void mp1_changed( MPTYPE& modelPointThatChanged ) noexcept
    {
        if ( modelPointThatChanged.isNotValid() == false )
        {
            m_masterThread.signal();
        }
    }
};

#endif // end header latch