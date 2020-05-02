#ifndef Cpl_TShell_Socket_h_
#define Cpl_TShell_Socket_h_
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

#include "Cpl/TShell/ProcessorApi.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Io/Socket/Listener.h"
#include "Cpl/Io/Socket/InputOutput.h"


///
namespace Cpl {
///
namespace TShell {

/** This concrete class provides a socket listener/server and 'threading wrapper' for
	running a TShell Command Processor. Only one socket connection is allowed at any
	given time to the Shell.

	NOTE: This class dynamically allocates memory and dynamically creates a 
	      Thread!
 */

class Socket : public Cpl::Io::Socket::Listener::Client
{
protected:
	/// Command Processor to run
	ProcessorApi&					m_shell;

	/// Thread that the shell runs in
	Cpl::System::Thread*            m_threadPtr;

	/// Thread priority to run the shell
	int                             m_priority;

	/// Thread name for the shell
	const char*                     m_name;

	/// Socket Listener (is ASSUMED to be a runnable instance)
	Cpl::Io::Socket::Listener&      m_listener;

	/// Socket stream
	Cpl::Io::Socket::InputOutput    m_stream;

	/// How I was created
	bool                            m_staticInstance;


public:
	/** Constructor.  The 'thisIsAStaticInstance' argument is to inform the
		instance being create that it is being created statically (i.e. before
		main() is entered) - which is the intended typically behavior.  This
		knowledge is used to inhibit delete/destroy-thread actions in the
		instance's destructor.  This is necessary because there is no
		guaranteed order to when static destructor fire and as such the state
		of the CPL Libraries static resources (e.g. mutexes) are unknown
		which your application will crash/behavior poorly on exit.
	 */
	Socket( ProcessorApi& shell, Cpl::Io::Socket::Listener& listener, const char* threadName = "TShell", int threadPriority = CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER, bool thisIsAStaticInstance=true ) noexcept;


	/// Destructor. Note: the referenced 'listener' is NOT destroyed/clean-up since it was NOT created by me.
	~Socket();


public:
	/** This method starts the Processor.  It is thread safe in that it
		runs in the context of the calling thread - and spawns a new
		thread for Shell/Command Processor to executing in.
	 */
	void launch( int portNumToListenOn ) noexcept;


public:
	/// See Cpl::Io::Socket::Listener::Client
	bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo );

};


};      // end namespaces
};
#endif  // end header latch
