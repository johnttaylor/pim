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

#include "MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Trace.h"


#define SECT_ "Cpl::Itc"


///
using namespace Cpl::Itc;

/////////////////////
MailboxServer::MailboxServer( unsigned long                       timingTickInMsec,
                              Cpl::System::SharedEventHandlerApi* eventHandler ) noexcept
    : Mailbox( *((Cpl::System::EventLoop*)this) )
    , Cpl::System::EventLoop( timingTickInMsec, eventHandler )
{
}


/////////////////////
void MailboxServer::appRun()
{
    startEventLoop();
    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
        if ( run )
        {
            processMessages();
        }
    }
    stopEventLoop();
}

