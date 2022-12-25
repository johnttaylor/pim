#ifndef Cpl_TShell_Processor_h_
#define Cpl_TShell_Processor_h_
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
#include "Cpl/TShell/Context_.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Frame/StreamEncoder.h"
#include "Cpl/Text/Frame/StreamDecoder.h"


/** This symbol defines the size, in bytes, of the maximum allowed input
	string/command.
 */
#ifndef OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE
#define OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE              128
#endif

 /** This symbol defines the size, in bytes, of the maximum allowed unframed
	 output string/command.
  */
#ifndef OPTION_CPL_TSHELL_PROCESSOR_OUTPUT_SIZE
#define OPTION_CPL_TSHELL_PROCESSOR_OUTPUT_SIZE             256
#endif

/** This symbols defines the Shell's greeting message
 */
#ifndef OPTION_CPL_TSHELL_PROCESSOR_GREETING
#define OPTION_CPL_TSHELL_PROCESSOR_GREETING                "\n--- Your friendly neighborhood TShell. ---\n\n\n"
#endif

/** This symbols defines the Shell's farewell message
 */
#ifndef OPTION_CPL_TSHELL_PROCESSOR_FAREWELL
#define OPTION_CPL_TSHELL_PROCESSOR_FAREWELL                "\n--- ...I am melting, am melting... ---\n\n"
#endif

/** This symbols defines the Shell's prompt string
 */
#ifndef OPTION_CPL_TSHELL_PROCESSOR_PROMPT
#define OPTION_CPL_TSHELL_PROCESSOR_PROMPT                  "$ "
#endif

///
namespace Cpl {
///
namespace TShell {

/** This concrete class provides the implementation of Command Processor for
	a TShell engine.

	The implementation assumes a single threaded model, i.e. the Command
	Processor and all of its  commands run in a single thread.  It is
	APPLICATION's responsibility to provide any desired multi-threaded
	support. There are two caveats to the single-threaded model:

		o The output of the commands are mutex protected.  This allows the
		  Output stream to be 'shared' with other sub-systems and/or
		  threads (e.g. the shell shares the same Output stream as the
		  Cpl::System::Trace logging output).

		o The stop() method can be called safely from other threads.


	Commands have the following syntax:

		o A command starts with a printable ASCII character and ends with
		  a newline.

		o Non-printable ASCII characters are not allowed.

		o Any line that starts with a '#' is treated as comment line and
		  is ignored.

		o Format of a command is: verb [arg]*  where the verb and arguments
		  are separated by spaces.  Arguments can contain spaces character
		  by enclosing the argument with double quote characters.  A double
		  quote character can be embedded inside a quoted string by preceding
		  it the double quote character with the escape character.  The escape
		  character can be embedded by escaping the escape character.

	The USE_CPL_TSHELL_PROCESSOR_SILENT_WHEN_PUBLIC switch is use
	to alter the TShell's output behavior - to not 'say anything'
	until a user has successfully been authenticated (using the 'User'
	command

	HOW TO Enable Security:

		- Set OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL to something
		  other than ePUBLIC.  This will be the permission for all legacy (i.e.
		  not Security aware) TShell commands

		- Recommend turning on the switch: USE_CPL_TSHELL_PROCESSOR_SILENT_WHEN_PUBLIC

		- Include the Cpl::TShell::Cmd::User command and provide an 
		  implementation of the Cpl::TShell::Security interface (that is passed 
		  to the command's constructor).

		- Optionally include new security aware commands
 */
class Processor : public Context_
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
	Processor( Cpl::Container::Map<Command>&     commands,
			   Cpl::Text::Frame::StreamDecoder&  deframer,
			   Cpl::Text::Frame::StreamEncoder&  framer,
			   Cpl::System::Mutex&               outputLock,
			   char                              commentChar='#',
			   char                              argEscape='`',
			   char                              argDelimiter=' ',
			   char                              argQuote='"',
			   char                              argTerminator='\n',
			   Security::Permission_T            initialPermissionLevel = Security::ePUBLIC
	);


public:
	/// See Cpl::TShell::ProcessorApi
	bool start( Cpl::Io::Input& infd, Cpl::Io::Output& outfd, bool blocking=true ) noexcept;

	/// See Cpl::TShell::ProcessorApi
	int poll() noexcept;

	/// See Cpl::TShell::ProcessorApi
	void requestStop() noexcept;

	/// See Cpl::TShell::ProcessorApi
	char getEscapeChar() noexcept;

	/// See Cpl::TShell::ProcessorApi
	char getDelimiterChar() noexcept;

	/// See Cpl::TShell::ProcessorApi
	char getQuoteChar() noexcept;

	/// See Cpl::TShell::ProcessorApi
	char getTerminatorChar() noexcept;

public:
	/// See Cpl::TShell::Context_
	Cpl::Container::Map<Command>& getCommands() noexcept;

	/// See Cpl::TShell::Context_
	bool writeFrame( const char* text ) noexcept;

	/// See Cpl::TShell::Context_
	bool writeFrame( const char* text, size_t maxBytes ) noexcept;

	/// See Cpl::TShell::Context_
	Cpl::Text::String& getOutputBuffer() noexcept;

	/// See Cpl::TShell::Context_
	Cpl::Text::String& getTokenBuffer() noexcept;

	/// See Cpl::TShell::Context_
	Cpl::Text::String& getTokenBuffer2() noexcept;

	/// See Cpl::TShell::Context_
	bool oobRead( void* buffer, int numBytes, int& bytesRead ) noexcept;

	/// See Cpl::TShell::Context_
	Security::Permission_T getUserPermissionLevel() const noexcept;

	/// See Cpl::TShell::Context_
	Security::Permission_T setUserPermissionLevel( Security::Permission_T newPermissionLevel ) noexcept;

protected:
	/** Helper method that attempts to execute the content of the de-framed/decoded
	    'inputString'.  The method returns the result code of the execute
   		command.  If 'inputString' is not a valid command, then the appropriate
		error/result code is returned.
	 */
	virtual Command::Result_T executeCommand( char* deframedInput, Cpl::Io::Output& outfd ) noexcept;

	/// Helper method
	bool outputCommandError( Command::Result_T result, const char* deframedInput ) noexcept;

	/** Helper method that performs a 'single' read cycle of the input stream.
		Returns 0 if successful. Returns 1 if exiting. Returns -1 on error
	 */
	int getAndProcessFrame( Cpl::Io::Output& outfd ) noexcept;

	/** Helper method that executes the decoder, i.e. logic to parse the incoming
	    text.  Returns 1 if a full/valid frame was found. Returns 0 if input frame
		is incomplete. Return -1 if an error occurred.
	 */
	virtual int readInput( size_t& frameSize ) noexcept;

protected:
	/// Command list
	Cpl::Container::Map<Command>&       m_commands;

	/// Raw input de-framer
	Cpl::Text::Frame::StreamDecoder&    m_deframer;

	/// Output framer handle
	Cpl::Text::Frame::StreamEncoder&    m_framer;

	/// Output lock
	Cpl::System::Mutex&                 m_outLock;

	/// User's permission level	
	Security::Permission_T				m_userPermLevel;

	/// Comment character
	char                                m_comment;

	/// Argument Escape character
	char                                m_esc;

	/// Argument delimiter
	char                                m_del;

	/// Argument quote character
	char                                m_quote;

	/// Argument terminator character
	char                                m_term;

	/// Set to true when 'command prompt' should be outputted
	bool								m_writeCommandPrompt;

	/// Current frame size
	size_t								m_frameSize;

	/// My run state
	bool                                m_running;

	/// Input Frame buffer
	char                                m_inputBuffer[OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE + 1];

	/// Buffer that is used to construct output messages
	Cpl::Text::FString<OPTION_CPL_TSHELL_PROCESSOR_OUTPUT_SIZE>  m_outputBuffer;

	/// Shared token work buffer
	Cpl::Text::FString<OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE>   m_tokenBuffer;

	/// Shared token work buffer
	Cpl::Text::FString<OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE>   m_tokenBuffer2;
};


};      // end namespaces
};
#endif  // end header latch
