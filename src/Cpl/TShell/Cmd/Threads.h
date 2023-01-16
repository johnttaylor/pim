#ifndef Cpl_TShell_Cmd_Threads_h
#define Cpl_TShell_Cmd_Threads_h
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

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"
#include "Cpl/Text/String.h"
#include "Cpl/System/Thread.h"



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
	/// The command verb/identifier
	static constexpr const char* verb = "threads";

	/// The command usage string
	static constexpr const char* usage = "threads";

	/** The command detailed help string (recommended that lines do not exceed 80 chars)
														  1         2         3         4         5         6         7         8
												 12345678901234567890123456789012345678901234567890123456789012345678901234567890
	 */
	static constexpr const char* detailedHelp = "  Displays the list of threads.";

public:
	/// See Cpl::TShell::Command
	const char* getUsage() const noexcept { return usage; }

	/// See Cpl::TShell::Command
	const char* getHelp() const noexcept { return detailedHelp; }


protected:
	/// Cache my Processor/Shell context when traversing the thread list
	Cpl::TShell::Context_*  m_contextPtr;

	/// Count of threads
	unsigned                     m_count;

	/// Cache IO status/errors
	bool                         m_io;


public:
	/// Constructor
	Threads( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
			 Security::Permission_T                     minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

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
