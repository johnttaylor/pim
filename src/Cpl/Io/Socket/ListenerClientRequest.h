#ifndef Cpl_Io_Socket_ListenerClientRequest_h_
#define Cpl_Io_Socket_ListenerClientRequest_h_
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

#include "Cpl/Io/Socket/Listener.h"
#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/SAP.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {

/** This abstract class define message types and payloads for a set of
	ITC services. The request() method(s) are to be implemented by the
	'server'
 */

class ListenerClientRequest
{
public:
	/// SAP for this API
	typedef Cpl::Itc::SAP<ListenerClientRequest> SAP;


public:
	/// Payload for Message: newConnection
	class NewConnectionPayload
	{
	public:
		/// Accepted socket descriptor (passed to client)
		Cpl::Io::Descriptor  m_acceptedFd;

		/// Raw Information about connection (passed to client)
		const char*          m_rawConnectionInfo;

		/// Client's acceptance flag/status (returned to the listener) 
		bool                 m_accepted;

	public:
		/// Constructor
		NewConnectionPayload( Cpl::Io::Descriptor newfd, const char* rawConnectionInfo )
			:m_acceptedFd( newfd ),
			m_rawConnectionInfo( rawConnectionInfo ),
			m_accepted( false )
		{
		}
	};


	/// Message Type: newConnection
	typedef Cpl::Itc::RequestMessage<ListenerClientRequest, NewConnectionPayload>  NewConnectionMsg;;



public:
	/// Request: NewConnection
	virtual void request( NewConnectionMsg& msg ) = 0;


public:
	///
	virtual ~ListenerClientRequest() {}
};





};      // end namespaces
};
};
#endif  // end header latch
