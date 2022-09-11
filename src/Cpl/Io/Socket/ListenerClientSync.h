#ifndef Cpl_Io_Socket_ListenerClientSync_h_
#define Cpl_Io_Socket_ListenerClientSync_h_
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
/** @file */

#include "Cpl/Io/Socket/Listener.h"
#include "Cpl/Io/Socket/ListenerClientRequest.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This partially concrete class implements the synchronous ITC open
	call for the OpenApi.  The subclass is still required to implement the ITC
	request() method.
 */
class ListenerClientSync : public Listener::Client,
	public ListenerClientRequest
{
protected:
	/// Mailbox (of the server implementing the ITC request() method
	Cpl::Itc::PostApi& m_mbox;


public:
	/// Constructor
	ListenerClientSync( Cpl::Itc::PostApi& myMbox );


public:
	/// See Cpl::Io::Socket::Listener::Client
	bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo );

};



};      // end namespaces
};
};
#endif  // end header latch
