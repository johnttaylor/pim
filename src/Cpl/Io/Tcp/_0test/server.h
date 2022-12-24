#ifndef Server_h_
#define Server_h_
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
#include "Cpl/Io/Tcp/AsyncListener.h"

#ifndef OPTION_TCP_TEST_PORT_NUM
#define OPTION_TCP_TEST_PORT_NUM    5002
#endif

int runTest( Cpl::Io::Tcp::AsyncListener& listener, int portNum );


#endif  // end header latch
