#ifndef Cpl_TShell_Command_h_
#define Cpl_TShell_Command_h_
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

#include "Cpl/Container/MapItem.h"
#include "Cpl/Io/Output.h"
#include "Cpl/TShell/Security.h"


///
namespace Cpl {
///
namespace TShell {

/// Forward class reference to avoid circular header includes
class Context_;


/** This class defines the interface for a TShell command.
 */
class Command : public Cpl::Container::MapItem
{
public:
	/// Possible result codes when executing a command
	enum Result_T {
		eSUCCESS            = 0,    //!< Command executed successfully
		eERROR_BAD_SYNTAX   = 1,    //!< The command string cannot be parsed
		eERROR_INVALID_CMD  = 2,    //!< Not a valid/supported command (Note: Typically this is only returned by the Command Processor - not individual commands)
		eERROR_IO           = 3,    //!< Command failed due to an error writing to the Output stream
		eERROR_MISSING_ARGS = 4,    //!< Command failed due to execute due to missing one or more required arguments
		eERROR_EXTRA_ARGS   = 5,    //!< Command failed due to execute due to unexpected 'extra' arguments
		eERROR_INVALID_ARGS = 6,    //!< Command failed due to execute due to one or more arguments being invalid
		eERROR_FAILED       = 7     //!< Command failed to complete one or more of actions.
	};



public:
	/** This method executes the command.
	 */
	virtual Result_T execute( Context_& context, char* rawCmdString, Cpl::Io::Output& outfd ) noexcept = 0;


public:
	/// This method returns the command's verb string
	virtual const char* getVerb() const noexcept = 0;


	/// This method returns the command's usage string
	virtual const char* getUsage() const noexcept = 0;


	/** This method returns the command's detailed help string.  Detailed
		help is optional.  If the command does not support detailed help,
		then 0 is returned.
	 */
	virtual const char* getHelp() const noexcept = 0;


	/** Returns the minimum required permission level needed to execute the 
	    command.
	 */
	virtual Security::Permission_T getMinPermissionRequired() const noexcept = 0;

protected:
	/// Constructor
	Command() {};

public:
	/// Virtual destructor
	virtual ~Command() {}

};

};      // end namespaces
};
#endif  // end header latch
