#ifndef client_h_
#define client_h_
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
#include "Cpl/Io/Tcp/AsyncConnector.h"

#ifndef OPTION_REMOTE_HOST_ADDRESS
#define OPTION_REMOTE_HOST_ADDRESS  "127.0.0.1"
#endif 

#ifndef OPTION_TCP_TEST_PORT_NUM
#define OPTION_TCP_TEST_PORT_NUM    5002
#endif

int runTest( Cpl::Io::Tcp::AsyncConnector& connector, const char* remoteHostAddr, int portNum );


#endif  // end header latch
