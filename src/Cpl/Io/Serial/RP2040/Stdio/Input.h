#ifndef Cpl_Io_Serial_RP2040_Stdio_Input_h_
#define Cpl_Io_Serial_RP2040_Stdio_Input_h_
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

#include "Cpl/Io/Input.h"



///
namespace Cpl {
///
namespace Io {
///
namespace Serial {
///
namespace RP2040 {
///
namespace Stdio {


/** This concrete class implements a Input stream using Raspberry Pi RP2040's
    SDK.  Multiple instances of this class can be created because there is
    only actual stdin stream.

    Notes: 
        1. The implement does NOT support the blocking semantics of the Cpl::Io
           streams.  The design decision was for the implementation to be 
           compatible/useful on bare-metal systems, i.e. no threads required.
        2. The Application is RESPONSIBLE for calling stdio_init_all() on 
           start-up of the application BEFORE any calls to this class (other 
           than the constructor).
 */
class Input : public Cpl::Io::Input
{
public:
    /// Constructor
    Input();

public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::Input::read;

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();

	/// See Cpl::Io::IsEos
	bool isEos();

	/// See Cpl::Io::Input
    void close();

protected:
    /// Cached byte when 'peeking' into the input stream
    int m_nextByte;
};


};      // end namespaces
};
};
};
};
#endif  // end header latch
