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


#define SECT_ "Cpl::Dm"


///
using namespace Cpl::Dm;

/////////////////////
MailboxServer::MailboxServer( unsigned long timingTickInMsec ) noexcept
    :Cpl::Dm::EventLoop( timingTickInMsec )
    ,Cpl::Itc::Mailbox( *((Cpl::System::EventLoop*)this) )
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
            processChangeNotifications();
            processMessages();
        }
    }
}
