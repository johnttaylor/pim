#ifndef Cpl_TShell_ContextApi_x_h_
#define Cpl_TShell_ContextApi_x_h_
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
#include "Cpl/TShell/Command.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl {
///
namespace TShell {

/** This Private Namespace class defines a "Context" for a TShell command. The
	Context provide common infrastructure, information, buffers, etc. that
	facilitates interaction between the Command Processor and individual
	commands.  The application SHOULD NEVER directly access this interface.
 */
class Context_ : public ProcessorApi
{
public:
	/// This method returns the list of implemented commands
	virtual Cpl::Container::Map<Command>& getCommands() noexcept = 0;

public:
	/// This method encodes and outputs the specified message/text.  The method returns false if there was Output Stream error
	virtual bool writeFrame( const char* text ) noexcept = 0;

	/// Same as writeFrame(), but only outputs (at most) 'N' bytes as the content of the frame
	virtual bool writeFrame( const char* text, size_t maxBytes ) noexcept = 0;


public:
	/** This method returns a working buffer for a command to format its
		output prior to 'writing the frame'.
	 */
	virtual Cpl::Text::String& getOutputBuffer() noexcept = 0;

	/** A shared/common working buffer. The buffer is guaranteed to be large
		enough hold any valid token from an input frame.  The contents of
		buffer is guaranteed to be empty/cleared.
	 */
	virtual Cpl::Text::String& getTokenBuffer() noexcept = 0;

	/** Same as getTokenBuffer(), except provides a second/separate token buffer
	 */
	virtual Cpl::Text::String& getTokenBuffer2() noexcept = 0;


public:
	/// Virtual destructor
	virtual ~Context_() {}
};


};      // end namespaces
};
#endif  // end header latch
