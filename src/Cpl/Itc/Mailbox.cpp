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

#include "Mailbox.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/GlobalLock.h"


///
using namespace Cpl::Itc;


////////////////////////////////////////////////////////////////////////////////
Mailbox::Mailbox( Cpl::System::EventLoop& myEventLoop )
    :m_eventLoop( myEventLoop )
{
}


void Mailbox::post( Message& msg ) noexcept
{
    // Update my internal FIFO
    Cpl::System::GlobalLock::begin();
    put( msg );
    Cpl::System::GlobalLock::end();

    // Wake up my event loop to process the message
    m_eventLoop.signal();
}

void Mailbox::postSync( Message& msg ) noexcept
{
    post( msg );
    CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT();
    Cpl::System::Thread::wait();
}


void Mailbox::processMessages() noexcept
{
    // Get the next message
    Cpl::System::GlobalLock::begin();
    Message* msgPtr = get();
    Cpl::System::GlobalLock::end();

    // Dispatch at MOST one message
    if ( msgPtr )
    {
        msgPtr->process();
    }
}


bool Mailbox::isPendingMessage() noexcept
{
    // Get the next message
    Cpl::System::GlobalLock::begin();
    Message* msgPtr = first();
    Cpl::System::GlobalLock::end();
    return msgPtr != nullptr;
}

