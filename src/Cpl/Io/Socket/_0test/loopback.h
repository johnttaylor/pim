#ifndef loopback_h_
#define loopback_h_
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

#include "Cpl/Io/Socket/Listener.h"
#include "Cpl/Io/Socket/Connector.h"

void initialize_loopback( Cpl::Io::Socket::Listener& listener, Cpl::Io::Socket::Connector& connector );


#endif  // end header latch
