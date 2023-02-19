#ifndef Cpl_TShell_Restarable_h_
#define Cpl_TShell_Restarable_h_
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

#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/Thread.h"


///
namespace Cpl {
///
namespace TShell {

/** This concrete class extends the 'Stdio' class for running a TShell Command 
	Processor that provides the ability to 'restart' the TShell to be 'restarted'.  

	NOTE: This class dynamically allocates memory and dynamically creates a 
	      Thread!
 */

class Restartable: public Stdio
{
public:
	/** Constructor.  See Cpl::TShell::Stdio for additional details.
	 */
	Restartable( Cpl::TShell::ProcessorApi& shell, const char* threadName = "TShell", int threadPriority = CPL_SYSTEM_THREAD_PRIORITY_NORMAL + CPL_SYSTEM_THREAD_PRIORITY_LOWER, bool thisIsAStaticInstance=true ) noexcept;


public:
	/** This method starts the Processor.  It is thread safe in that it
		runs in the context of the calling thread - and spawns a new
		thread for Shell/Command Processor to executing in.
	 */
	void launch( Cpl::Io::Input& infd, Cpl::Io::Output& outfd, bool enabledOnLaunch=true ) noexcept;

	/** This method is used to restart the shell. This method can ONLY be
		called AFTER launch has been called.
	 */
	void restart() noexcept;

	/** This method is used to request that the TShell be stopped AND optionally 
		that its thread to be terminated.

		Note: The shell self terminates when a 'bye' command was issued by the
		      user or a IO error on the shell's IO Streams are encounter.  When
			  the shell self terminates - the thread is placed into a blocking
			  state, i.e. waiting for a call to restart().
	 */
	void pleaseStop( bool stopThread = false ) noexcept;

	/** This method returns true when the shell is running
	 */
	bool isRunning() noexcept;
};


};      // end namespaces
};
#endif  // end header latch
