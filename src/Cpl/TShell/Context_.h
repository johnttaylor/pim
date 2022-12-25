#ifndef Cpl_TShell_ContextApi_x_h_
#define Cpl_TShell_ContextApi_x_h_
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

#include "Cpl/TShell/ProcessorApi.h"
#include "Cpl/TShell/Security.h"
#include "Cpl/TShell/Command.h"
#include "Cpl/Container/Map.h"


///
namespace Cpl {
///
namespace TShell {

/** This Private Namespace class defines a "Context" for a TShell command. The
    Context provide common infrastructure, information, buffers, etc. that
    facilitates interaction between the Command Processor and individual
    commands.  The application SHOULD NEVER directly access this interface.
 */
class Context_ : public ProcessorApi
{
public:
    /// This method returns the list of implemented commands
    virtual Cpl::Container::Map<Command>& getCommands() noexcept = 0;

public:
    /// This method encodes and outputs the specified message/text.  The method returns false if there was Output Stream error
    virtual bool writeFrame( const char* text ) noexcept = 0;

    /// Same as writeFrame(), but only outputs (at most) 'N' bytes as the content of the frame
    virtual bool writeFrame( const char* text, size_t maxBytes ) noexcept = 0;


public:
    /** This method returns a working buffer for a command to format its
        output prior to 'writing the frame'.
     */
    virtual Cpl::Text::String& getOutputBuffer() noexcept = 0;

    /** A shared/common working buffer. The buffer is guaranteed to be large
        enough hold any valid token from an input frame.  The contents of
        buffer is guaranteed to be empty/cleared.
     */
    virtual Cpl::Text::String& getTokenBuffer() noexcept = 0;

    /** Same as getTokenBuffer(), except provides a second/separate token buffer
     */
    virtual Cpl::Text::String& getTokenBuffer2() noexcept = 0;


public:
    /** This method allows a command access to the input stream, i.e. consume
        additional input data that is 'out-of-band' of the nominal newline
        delineated command syntax.

        Attempts to read the specified number of bytes from the stream into the
        supplied buffer.  The actual number of bytes read is returned via
        'bytesRead'. Returns true if successful, or false if End-of-Stream
        was encountered.
     */
    virtual bool oobRead( void* buffer, int numBytes, int& bytesRead ) noexcept = 0;

public:
    /** This method returns the 'logged user' permission level
     */
    virtual Security::Permission_T getUserPermissionLevel() const noexcept = 0;

    /** This method is used to updated the 'logged user' permission level.  The
        method returns the previous value for the user's permission level.

        NOTE: The program model is that all command implementations are well 
              behaved in that they do not indiscriminately call this method.  
              In practice only the 'user' command should every call this method!
     */
    virtual Security::Permission_T setUserPermissionLevel( Security::Permission_T newPermissionLevel ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~Context_() {}
};


};      // end namespaces
};
#endif  // end header latch
