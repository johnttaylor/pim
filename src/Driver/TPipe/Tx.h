#ifndef Driver_Tpipe_Tx_h_
#define Driver_Tpipe_Tx_h_
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

#include "Cpl/Text/String.h"
#include <string.h>

///
namespace Driver {
///
namespace TPipe {

/** This abstract class defines the 'Transmit Command' interface for the
    TPipe.  
 */
class Tx
{
public:
    /** Synchronously transmits a complete 'text command'.  The method will
        provide the proper TPipe framing and send the framed contents through
        the pipe.

        The method returns true once the frame data has been transferred to
        the TPipe's outbound stream. If an a Stream error occurred the method
        returns false.

        NOTE: The method will block if there is currently a transmission of a 
              frame in progress.  
     */
    virtual bool sendCommand( const char* completeCommandText, size_t numBytes ) noexcept = 0;

    /// Convenience method to send a command that is contain in Text string object
    inline bool sendCommand( Cpl::Text::String& command ) noexcept
    {
        return sendCommand( command.getString(), command.length() );
    }

    /// Convenience method to send a command that is a contained in a null terminated string
    inline bool sendCommand( const char* commandString ) noexcept
    {
        return sendCommand( commandString, strlen( commandString ) );
    }

    /** This is an OPTIMIZATION - use with care!!

        The method sends the data un-encoded to speed up the transfer of large
        commands.  THe application is responsible for managing the framing
     */
    virtual bool sendRawCommand( const char* completeCommandText, size_t numBytes ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~Tx() {}
};


};      // end namespaces
};
#endif  // end header latch
