#ifndef Cpl_Io_Socket_Win32_Connector_h_
#define Cpl_Io_Socket_Win32_Connector_h_
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

#include "Cpl/Io/Socket/Connector.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {
///
namespace Win32 {


/** This concrete class implements the Cpl::Io::Socket::Connector interface
	using IPv4/IPv6 addressing for the Win32 (aka winsock2) platform.
 */
class Connector : public Cpl::Io::Socket::Connector

{
public:
	/// Constructor
	Connector();

	/// Destructor
	~Connector();


public:
	/// See Cpl::Io::Socket::Connector
	Result_T establish( const char* remoteHostName, int portNumToConnectTo, Cpl::Io::Descriptor& fdOut );

};


};      // end namespaces
};
};
};
#endif  // end header latch
