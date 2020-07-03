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

#include "RecordServer.h"
#include "Cpl/System/Assert.h"

#define SECT_ "Cpl::Persistent"


///
using namespace Cpl::Persistent;

/////////////////////
RecordServer::RecordServer( Record* recordList[], unsigned long timingTickInMsec ) noexcept
    : Cpl::Dm::MailboxServer( timingTickInMsec )
    , Cpl::Itc::CloseSync( *((Cpl::Itc::PostApi*)this) )
    , m_records( recordList )
    , m_opened( false )
{
    CPL_SYSTEM_ASSERT( recordList );
}

void RecordServer::request( OpenMsg& msg )
{
    if ( !m_opened )
    {
        m_opened = true;

        // Start each record
        for ( unsigned i=0; m_records[i]; i++)
        {
            m_records[i]->start( *this );
        }
    }
    msg.returnToSender();
}

void RecordServer::request( CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;

        // Stop each record
        for ( unsigned i=0; m_records[i]; i++ )
        {
            m_records[i]->stop();
        }
    }
    msg.returnToSender();
}

