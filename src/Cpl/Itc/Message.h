#ifndef Cpl_Itc_Message_h_
#define Cpl_Itc_Message_h_
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

#include "Cpl/Container/Item.h"

///
namespace Cpl {
///
namespace Itc {

/** This abstract class defines the operations for an ITC message
 */
class Message : public Cpl::Container::ExtendedItem
{
public:
    /** This abstract operation is invoked by the mailbox owner to cause
        the message to be processed after it has been retrieved from the
        mailbox.
     */
    virtual void process() noexcept=0;


public:
    /// Virtual destructor
    virtual ~Message() {}
};

};      // end namespaces
};
#endif  // end header latch
