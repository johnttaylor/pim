#ifndef Cpl_Io_Serial_Arduino_InputOutput_h
#define Cpl_Io_Serial_Arduino_InputOutput_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Io/InputOutput.h"
#include "Bsp/Api.h"

///
namespace Cpl {
///
namespace Io {
///
namespace Serial {
///
namespace Arduino {


/** This concrete class implements the InputOutput stream interface
	as a wrapper around Arduino Serial class.

	NOTES:
	1) The read() methods are non-blocking, i.e. if there is no data
	   available - the methods will return 'true' but no data will be
	   read in.
	2) The write() methods are blocking in the sense that they will
	   not return until all of the specified data has been sent to
	   the UART Hardware peripheral, BUT it is non-blocking in the sense
	   the it does a busy-wait on the UART Hardware instead of thread
	   yielding.
 */

class InputOutput : public Cpl::Io::InputOutput
{
protected:
	/// Flag to keep track of started/stopped state
	bool            m_started;


public:
	/** Constructor.
	 */
	InputOutput(); 

	/// Destructor
	virtual ~InputOutput( void );


public:
	/** This method starts UART and must be called BEFORE any of
		InputOutput methods are called. This allows the instance to
		be statically created and the initialized/started once the
		'system is running' (aka in the Arduino setup() method). It
		is the same as calling 'Serial.begin();

		Note: See the Bsp/Api.h head file for
			  valid 'config' settings.

		Note: If using an instance of this class for CPL Tracing logging, then
			  the start() method MUST be called BEFORE the Cpl::System::Initialize()
			  method is called.
	 */
	void start( unsigned long baudrate=115200, uint16_t config = SERIAL_8N1 ) noexcept;

	/** This method return true when UART is 'ready'
	 */
	bool isReady( void );

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

	/// See Cpl::Io::Output.  This method does nothing.  See comments about the semantics of the write() methods
	void flush();

	/// See Cpl::Io::IsEos.
	bool isEos();
	
	/// See Cpl::Io::Output.  Note: An can be 're-opened' by calling the start() method.
	void close();

};

};      // end namespaces
};
};
};
#endif  // end header latch
