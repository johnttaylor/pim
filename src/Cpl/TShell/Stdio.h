#ifndef Cpl_TShell_Stdio_h_
#define Cpl_TShell_Stdio_h_
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


///
namespace Cpl {
///
namespace TShell {

/** This concrete class provides the 'threading wrapper' for running a
	TShell Command Processor.  This requires that the Input/Output streams
	be provided when the Shell is launched.

	NOTE: This class dynamically allocates memory and dynamically creates a 
	      Thread!
 */

class Stdio
{
protected:
	/// Command Processor to run
	ProcessorApi&			m_shell;

	/// Thread that the shell runs in
	Cpl::System::Thread*    m_threadPtr;

	/// Thread priority to run the shell
	int                     m_priority;

	/// Thread name for the shell
	const char*             m_name;

	/// Runnable instance
	Cpl::System::Runnable*  m_runnablePtr;

	/// How I was created
	bool                    m_staticInstance;

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
	Stdio( ProcessorApi& shell, const char* threadName = "TShell", int threadPriority = CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER, bool thisIsAStaticInstance=true ) noexcept;


	/// Destructor
	~Stdio();


public:
	/** This method starts the Processor.  It is thread safe in that it
		runs in the context of the calling thread - and spawns a new
		thread for Shell/Command Processor to executing in.
	 */
	void launch( Cpl::Io::Input& infd, Cpl::Io::Output& outfd ) noexcept;
};


};      // end namespaces
};
#endif  // end header latch
