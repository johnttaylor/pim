#ifndef Cpl_Io_Socket_InputOutput_h_
#define Cpl_Io_Socket_InputOutput_h_
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

#include "Cpl/Io/InputOutput.h"
#include "Cpl/Io/Descriptor.h"

///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This concrete class provides a platform independent 'standard'
	implementation of an InputOutput stream object where the stream
	is Socket connection.
 */
class InputOutput : public Cpl::Io::InputOutput
{
protected:
	/// Socket handle
	Cpl::Io::Descriptor  m_fd;

	/// End-of-Stream status
	bool                 m_eos;

public:
	/** Constructor. No 'fd' provided -->the instance MUST be activated prior
		to use.
	 */
	InputOutput();


	/** Constructor. 'fd' is a the 'socket descriptor' of a existing/opened
		socket
	 */
	InputOutput( Cpl::Io::Descriptor fd );


	/// Destructor
	~InputOutput( void );


public:
	/** Activates the stream, i.e. initializes the instance's underlying
		fd/stream descriptor. If instance's 'fd' is not in the closed state when
		this method is called a fatal error is generated.
	 */
	void activate( Cpl::Io::Descriptor fd );


public:
	/// Pull in overloaded methods from base class
	using Cpl::Io::InputOutput::read;

	/// See Cpl::Io::Input
	bool read( void* buffer, int numBytes, int& bytesRead );

	/// See Cpl::Io::Input
	bool available();


public:
	/// Pull in overloaded methods from base class
	using Cpl::Io::InputOutput::write;

	/// See Cpl::Io::Output
	bool write( const void* buffer, int maxBytes, int& bytesWritten );

	/// See Cpl::Io::Output
	void flush();

	/// See Cpl::Io::IsEos
	bool isEos();

	/// See Cpl::Io::Output
	void close();

};

};      // end namespaces
};
};
#endif  // end header latch
