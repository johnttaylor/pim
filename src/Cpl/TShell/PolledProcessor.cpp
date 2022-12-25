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

#include "PolledProcessor.h"
#include "Cpl/System/Assert.h"

///
using namespace Cpl::TShell;


///////////////////////////////////
PolledProcessor::PolledProcessor( Cpl::Container::Map<Command>&     commands,
	Cpl::Text::Frame::StreamDecoder&  deframer,
	Cpl::Text::Frame::StreamEncoder&  framer,
	Cpl::System::Mutex&               outputLock,
	char                              commentChar,
	char                              argEscape,
	char                              argDelimiter,
	char                              argQuote,
	char                              argTerminator,
	Security::Permission_T            initialPermissionLevel
)
	: Processor( commands, deframer, framer, outputLock, commentChar, argEscape, argDelimiter, argQuote, argTerminator, initialPermissionLevel )
	, m_outFd( nullptr )
{
}

int PolledProcessor::poll() noexcept
{
	CPL_SYSTEM_ASSERT( m_outFd );
	return getAndProcessFrame( *m_outFd );
}

bool PolledProcessor::start( Cpl::Io::Input & infd, Cpl::Io::Output & outfd, bool blocking_notUsed ) noexcept
{
	m_outFd = &outfd;
	return Processor::start( infd, outfd, false );
}

int PolledProcessor::readInput( size_t& frameSize ) noexcept
{
	bool isEof;
	if ( !m_deframer.scan( OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE, m_inputBuffer, frameSize, isEof ) )
	{
		// Error reading raw input -->exit the Command processor
		return -1;
	}

	return isEof? 1: 0;
}