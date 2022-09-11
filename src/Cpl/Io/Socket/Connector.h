#ifndef Cpl_Io_Socket_Connector_h_
#define Cpl_Io_Socket_Connector_h_
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

#include "Cpl/Io/Descriptor.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines the interface for establishing/requesting
	a SIMPLE socket connection, i.e. make a "client connection".  A single
	instance can be used to create many connections.
 */
class Connector

{
public:
	/// Possible return codes when attempting to establish a connection
	enum Result_T {
		eSUCCESS=0,     /// Connection was successful
		eERROR,         /// Error occurred
		eREFUSED,       /// Connection request was refused by the remote Host
	};


public:
	/** Requests a client connection to the specified remote Host.  Upon
		success an opened Cpl::Io::Descriptor is returned (via 'fdOut') for the
		connection.
	 */
	virtual Result_T establish( const char* remoteHostName, int portNumToConnectTo, Cpl::Io::Descriptor& fdOut ) = 0;


public:
	/// Virtual destructor
	virtual ~Connector() {}
};


};      // end namespaces
};
};
#endif  // end header latch
