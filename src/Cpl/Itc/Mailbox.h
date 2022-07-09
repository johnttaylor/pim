#ifndef Cpl_Itc_Mailbox_h_
#define Cpl_Itc_Mailbox_h_
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
/** @file */

#include "Cpl/Itc/PostApi.h"
#include "Cpl/Container/SList.h"
#include "Cpl/System/EventLoop.h"


///
namespace Cpl {
///
namespace Itc {

/** This mailbox class implements an Inter Thread Communications message
    queue. There is no limit to the number of messages that can be stored in
    the queue at any given time since the FIFO queue and the messages uses the
    intrusive container mechanisms from the Cpl::Container namespace.
 */

class Mailbox :
    public PostApi,
    public Cpl::Container::SList<Message>
{
public:
    /// Constructor
    Mailbox( Cpl::System::EventLoop& myEventLoop );


public:
    /// See Cpl::Itc::PostApi
    void post( Message& msg ) noexcept;

    /// See Cpl::Itc::PostApi
    void postSync( Message& msg ) noexcept;


protected:
    /** This operation is used process any pending messages.
     */
    virtual void processMessages() noexcept;


protected:
    /// The EventLoop that I wait-on/dispatch-msgs-from
    Cpl::System::EventLoop& m_eventLoop;

};


};      // end namespaces
};
#endif  // end header latch