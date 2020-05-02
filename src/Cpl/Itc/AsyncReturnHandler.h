#ifndef Cpl_Itc_AsyncReturnHandler_h_
#define Cpl_Itc_AsyncReturnHandler_h_
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

#include "Cpl/Itc/ReturnHandler.h"
#include "Cpl/Itc/PostApi.h"

///
namespace Cpl {
///
namespace Itc {


/** This class implements an asynchronous ReturnHandler. When the rts() method
    of the message associated with this class is invoked, the message referenced
    as a member variable of this class is posted to the mailbox referenced as
    a member variable of this class. It is expected that the referenced mailbox
    belongs to the client (i.e. the sender of the original message), and that
    the message contains a reference to the original message, such that the
    original message can be released by the client.
 */
class AsyncReturnHandler : public ReturnHandler
{
protected:
    /** Refers to the mailbox to which the response message will be posted.
     */
    PostApi & m_mbox;

    /** Refers to the response message to be posted to the response mailbox.
     */
    Message&    m_msg;

public:
    /** The constructor initializes the response mailbox and message references.
     */
    AsyncReturnHandler( PostApi& mbox, Message& msg );


public:
    /// See ReturnHandler
    void    rts() noexcept;
};

};      // end namespaces
};
#endif  // end header latch
