#ifndef Cpl_System_EventFlag_h_
#define Cpl_System_EventFlag_h_
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
#include <stdint.h>

/// Default the Event Flags data type to 32 Events flags 
#ifndef Cpl_System_EventFlag_T
#define Cpl_System_EventFlag_T            uint32_t
#endif

///
namespace Cpl {
///
namespace System {



/** This abstract class defines the interface to generated a 'Event Flag'. Event
    Flags are used to indicate that an 'event' has occurred.  Each Event Flag
    is represent by a bit the Cpl_System_Event_Flags_T data type.

    Individual event flags can be viewed as binary semaphores with respect
    to be signaled/waiting (though waiting is done on the thread's entire set
    of event flags).

    A thread (runnable object) can wait for at least one event to be signaled.
    When the thread is waiting on event(s) and it is then signaled - all of
    Events that were in the signaled state when the thread was unblock are
    cleared.
 */
class EventFlag
{
public:
    /** This operation is called by clients to set one or more Event Flags.
        Each bit in 'events' is different Event Flag.

        This method can ONLY be called from a thread context.
     */
    virtual void notifyEvents( Cpl_System_EventFlag_T events ) noexcept = 0;

    /** This operation is similar to notifyEvents(), except that it sets one
        event flags.  The 'eventNumber' is the bit number (zero based) of the
        Event Flag to set.

        This method can ONLY be called from a thread context.
     */
    virtual void notify( uint8_t eventNumber ) noexcept = 0;


public:
    /** This method is same as notifyEvents() EXCEPT this method can ONLY be
        called from supervisor mode and/or ISR contexts.
     */
    virtual void su_notifyEvents( Cpl_System_EventFlag_T events ) noexcept = 0;

    /** This method is same as notify() EXCEPT this method can ONLY be
        called from supervisor mode and/or ISR contexts.
     */
    virtual void su_notify( uint8_t eventNumber ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~EventFlag() {}
};

};      // end namespaces
};
#endif  // end header latch

