#ifndef Cpl_Io_Socket_Listener_h_
#define Cpl_Io_Socket_Listener_h_
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

#include "Cpl/Io/Descriptor.h"
#include "Cpl/System/Runnable.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines the interface for a SIMPLE socket listener. A
	socket listener 'listens' for potential TCP/IP socket connections.  When a
	request for a connection comes in, the listener notifies the client.  It is
	the client's responsibility to determine if the connection request is
	accepted as well as provide memory for the accepted connection. The client
	is also responsible for reclaiming the memory once the socket connection has
	been terminated.

	The socket listener is designed to run it is own thread.  The  callback
	mechanism uses the Cpl::Itc Request mechanism, i.e. the client  implementing
	the callback must be running in a thread that is using/has a
	Cpl::Itc::MailboxServer.

	NOTE: The application is responsible for creating the thread for the
		  listener to run in.
 */
class Listener : public Cpl::System::Runnable

{
public:
	/** This class defines the callback mechanism used for accepting incoming
		socket connections.

		NOTE: Typically the client should NOT inherit/implement this class
			  directly  because the callbacks are across threads.  The client
			  should inherit from the ListenerClienSync class and then implement
			  the request(msg) methods associated with actions below.
	 */
	class Client
	{
	public:
		/** This method is a callback method that is called when the listener
			has  accepted in incoming socket request.  It is up the client to
			determine  if the application will accept or reject the socket.  If
			the client rejects the socket, it needs to return false, else
			returns true.
		 */
		virtual bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo ) = 0;


	public:
		/// Virtual destructor
		virtual ~Client() {}
	};



public:
	/// Starts the Listener listening.  
	virtual void startListening( Client& client, int portNumToListenOn ) = 0;


	/** Shuts down the Listener.  The client specified in the startListening()
		method will be notified when the shutdown has completed.
	 */
	virtual void terminate() = 0;


public:
	/// Virtual destructor
	virtual ~Listener() {}
};


};      // end namespaces
};
};
#endif  // end header latch
