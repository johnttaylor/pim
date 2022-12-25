#ifndef Cpl_TShell_PolledProcessor_h_
#define Cpl_TShell_PolledProcessor_h_
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

#include "Cpl/TShell/Processor.h"



	 ///
namespace Cpl {
///
namespace TShell {

/** This concrete class provides a non-blocking implementation of Command 
	PolledProcessor for a TShell engine.  See Cpl::TShell::PolledProcessor for 
	additional details.
 */
class PolledProcessor : public Processor
{
public:
	/** Constructor.

		@param commands					Set of supported commands
		@param deframer					Frame decoder used to identify individual command
										strings within the raw Input stream
		@param framer					Frame encoder used to encapsulate the output of
										command in the Output stream.
		@param outputLock				Mutex to be used for ensuring the atomic output
										of the commands.
		@param commentChar				The comment character used to indicate that the
										input string is a comment and should not be
										executed.
		@param argEscape				Escape character to be used when escaping double
										quote characters inside a quoted argument.
		@param argDelimiter				The delimiter character used to separate the
										command verb and arguments.
		@param argQuote					The quote character used to 'double quote' a
										argument string.
		@param argTerminator			The command terminator character.
		@param initialPermissionLevel   The initial minimum permission level that a user needs to issue command(s)

	 */
	PolledProcessor( Cpl::Container::Map<Command>&     commands,
					 Cpl::Text::Frame::StreamDecoder&  deframer,
					 Cpl::Text::Frame::StreamEncoder&  framer,
					 Cpl::System::Mutex&               outputLock,
					 char                              commentChar='#',
					 char                              argEscape='`',
					 char                              argDelimiter=' ',
					 char                              argQuote='"',
					 char                              argTerminator='\n',
					 Security::Permission_T            initialPermissionLevel = Security::ePUBLIC );


public:
	/// See Cpl::TShell::ProcessorApi
	bool start( Cpl::Io::Input& infd, Cpl::Io::Output& outfd, bool blocking=true ) noexcept;
	
	/// See Cpl::TShell::ProcessorApi
	int poll() noexcept;

protected:
	/** Helper method that executes the decoder, i.e. logic to parse the incoming
		text.  Returns 1 if a full/valid frame was found. Returns 0 if input frame
		is incomplete. Return -1 if an error occurred.
	 */
	int readInput( size_t& frameSize ) noexcept;

protected:
	/// Cached output stream pointer 
	Cpl::Io::Output*	m_outFd;
};


};      // end namespaces
};
#endif  // end header latch
