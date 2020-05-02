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

#include "ListenerClientSync.h"
#include "Cpl/Itc/SyncReturnHandler.h"


///
using namespace Cpl::Io::Socket;


///////////////////////
ListenerClientSync::ListenerClientSync( Cpl::Itc::PostApi& myMbox )
	:m_mbox( myMbox )
{
}


///////////////////////
bool ListenerClientSync::newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo )
{
	NewConnectionPayload        payload( newFd, rawConnectionInfo );
	Cpl::Itc::SyncReturnHandler srh;
	NewConnectionMsg            msg( *this, payload, srh );
	m_mbox.postSync( msg );

	return payload.m_accepted;
}
