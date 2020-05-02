#ifndef Cpl_TShell_ProcessorApi_h_
#define Cpl_TShell_ProcessorApi_h_
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

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"


///
namespace Cpl {
///
namespace TShell {

/** This class defines the interface a TShell Command Processor.  The
	Command Processor is responsible for scanning the input, determining
	what command (if any) to execute; and then executing the command.
 */

class ProcessorApi
{
public:
	/** This method is used to start the Command Processor, i.e. it will
		begin to process commands.  This command will not return until
		the Command Processor self terminates or a Input/Output stream
		error was encounter.  The method returns true if the Command Processor
		self terminated or was requested to stop; else false is returned (i.e.
		a Input/Output stream error was encounter).

		NOTE: This method is an 'in-thread' initialization, i.e. not thread
		safe.  The application is RESPONSIBLE for managing threading issues.
	 */
	virtual bool start( Cpl::Io::Input& infd, Cpl::Io::Output& outfd ) noexcept = 0;


	/** This non-blocking method requests the Command Processor to stop.  When
		(or if) the Command Processor actually stops depends on the target's
		implementation, health of the Shell, current command(s) executing, etc.
		This method returns immediately.  There is no feedback/confirmation
		when the Command Processor stops.
	 */
	virtual void requestStop() noexcept = 0;

public:
	/// Getter for escape character
	virtual char getEscapeChar() noexcept = 0;

	/// Getter for delimiter character
	virtual char getDelimiterChar() noexcept = 0;

	/// Getter for quote character
	virtual char getQuoteChar() noexcept = 0;

	/// Getter for terminator character
	virtual char getTerminatorChar() noexcept = 0;

public:
	/// Virtual destructor
	virtual ~ProcessorApi() {}
};


};      // end namespaces
};
#endif  // end header latch
