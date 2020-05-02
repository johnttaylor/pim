#ifndef Cpl_Io_Socket_Listener_x_h_
#define Cpl_Io_Socket_Listener_x_h_
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
#include "Cpl/System/Thread.h"
#include "Cpl/System/Mutex.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This private, mostly concrete helper class manages the top level threading
	aspects for a simple listener.
 */
class Listener_ : public Listener
{
protected:
	/// Port Number to listen on
	int                     m_socket;

	/// Client
	Listener::Client*       m_clientPtr;

	/// Used to synchronize the startListening command
	Cpl::System::Signable*  m_myThreadPtr;

	/// Mutex to guard start up sequence
	Cpl::System::Mutex      m_lock;

	/// Indicates if the listener was instructed to start
	bool					m_startCalled;


protected:
	/// Constructor
	Listener_();


public:
	/// See Cpl::Io::Socket::Listener
	void startListening( Listener::Client& client, int portNumToListenOn );

	/// See Cpl::Io::Socket::Listener
	void terminate();


protected:
	/// See Cpl::System::Runnable
	void appRun();

	/// See Cpl::System::Runnable
	void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr );

	/// See Cpl::System::Runnable
	void pleaseStop();

protected:
	/// Begin listening -->to be implemented by platform specific child class
	virtual void listen() =0;

	/// Stop the listener -->to be implemented by platform specific child class
	virtual void stopListener() =0;
};



};      // end namespaces
};
};
#endif  // end header latch
