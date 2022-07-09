#ifndef Cpl_TShell_Cmd_Threads_h
#define Cpl_TShell_Cmd_Threads_h
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

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"
#include "Cpl/Text/String.h"
#include "Cpl/System/Thread.h"



/** Command
										"         1         2         3         4         5         6         7         8"
										"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLCMD_CMD_THREADS_	    "threads"
/// Usage
#define CPLTSHELLCMD_USAGE_THREADS_     "threads"

/// Detailed Help text
#ifndef CPLTSHELLCMD_DETAIL_THREADS_
#define CPLTSHELLCMD_DETAIL_THREADS_    "  Displays the list of threads."


#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Cpl {
///
namespace TShell {
///
namespace Cmd {



/** This class implements a  Shell command
 */
class Threads : public Command, public Cpl::System::Thread::Traverser
{
public:
	/// See Cpl::TShell::Command
	const char* getUsage() const noexcept { return CPLTSHELLCMD_USAGE_THREADS_; }

	/// See Cpl::TShell::Command
	const char* getHelp() const noexcept { return CPLTSHELLCMD_DETAIL_THREADS_; }


protected:
	/// Cache my Processor/Shell context when traversing the thread list
	Cpl::TShell::Context_*  m_contextPtr;

	/// Count of threads
	unsigned                     m_count;

	/// Cache IO status/errors
	bool                         m_io;


public:
	/// Constructor
	Threads( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;

public:
	/// See Cpl::TShell::Command
	Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;



public:
	/// See Cpl::System::Thread::Traverser
	Cpl::Type::Traverser::Status_T item( Cpl::System::Thread& nextThread );

protected:
	/// Hook when generating list header1 (i.e. subclass appends to the String)
	virtual void hookHeader1( Cpl::Text::String& text );

	/// Hook when generating list header2 (i.e. subclass appends to the String)
	virtual void hookHeader2( Cpl::Text::String& text );

	/// Hook when generating a thread row entry (i.e. subclass appends to the String)
	virtual void hookThreadEntry( Cpl::Text::String& text, Cpl::System::Thread& currentThread );
};

};      // end namespaces
};
};
#endif  // end header latch
