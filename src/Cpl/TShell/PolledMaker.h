#ifndef Cpl_TShell_PolledMaker_h_
#define Cpl_TShell_PolledMaker_h_
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

#include "Cpl/TShell/PolledProcessor.h"
#include "Cpl/Text/Frame/LineDecoder.h"
#include "Cpl/System/Private_.h"

///
namespace Cpl {
///
namespace TShell {


/** This concrete class is a "Maker" that assembles the objects needed
	for a 'basic' TShell Processor engine that has POLLED semantics
 */
class PolledMaker
{
protected:

	/// Framer for the output
	Cpl::Text::Frame::StreamEncoder m_framer;

	/// De-framer for the input stream
	Cpl::Text::Frame::LineDecoder<OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE> m_deframer;

	/// Command Processor
	PolledProcessor                 m_processor;


public:
	/** Constructor.  The application is responsible for supplying the set of commands and the mutex to ensure atomic
		output.
	 */
	PolledMaker( Cpl::Container::Map<Command>& cmdlist, Cpl::System::Mutex& lock=Cpl::System::Locks_::tracingOutput() );


public:
	/// Returns a reference to the Command processor
	ProcessorApi & getCommandProcessor( void ) noexcept { return m_processor; }

	/// Cast-operator: Short-hand for getCommandProcessor()
	operator ProcessorApi& ( ) { return m_processor; }

};


};      // end namespaces
};
#endif  // end header latch
