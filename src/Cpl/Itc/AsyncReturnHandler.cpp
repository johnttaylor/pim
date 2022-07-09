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

#include "AsyncReturnHandler.h"
#include "Message.h"
#include "PostApi.h"

//
using namespace Cpl::Itc;


//////////////////////////////////////
AsyncReturnHandler::AsyncReturnHandler( PostApi& mbox, Message& msg )
    :m_mbox( mbox ),
    m_msg( msg )
{
}

void AsyncReturnHandler::rts() noexcept
{
    m_mbox.post( m_msg );
}

