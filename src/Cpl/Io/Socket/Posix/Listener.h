#ifndef Cpl_Io_Socket_Listener_Posix_h_
#define Cpl_Io_Socket_Listener_Posix_h_
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


#include "colony_config.h"
#include "Cpl/Io/Socket/Listener_.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {
///
namespace Posix {


/** This value is number of retries that is performed when attempting
	to bind to the listening port.
 */
#ifndef USE_CPL_IO_SOCKET_POSIX_BIND_RETRIES
#define USE_CPL_IO_SOCKET_POSIX_BIND_RETRIES        5
#endif

 /** This value is time, in milliseconds between retries during the binding
	 process.
  */
#ifndef USE_CPL_IO_SOCKET_POSIX_BIND_RETRY_WAIT
#define USE_CPL_IO_SOCKET_POSIX_BIND_RETRY_WAIT     (10*1000)
#endif



  /** This concrete class implements the Cpl::Io::Socket::Listener interface
	  using IPv4 addressing for the Posix compliant platform.
   */
class Listener : public Cpl::Io::Socket::Listener_
{
protected:
	/// socket I am listen on
	int             m_fd;

public:
	/// Constructor
	Listener();

	/// Destructor
	~Listener();


protected:
	/// See Cpl::Io::Socket::Listener_
	virtual void listen();

	/// See Cpl::Io::Socket::Listener_
	virtual void stopListener();
};



};      // end namespaces
};
};
};
#endif  // end header latch
