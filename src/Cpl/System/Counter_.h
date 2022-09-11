#ifndef Cpl_System_Counter_h_
#define Cpl_System_Counter_h_
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
namespace System {


/** This abstract class defines the call-back interface for a Counter object.
    A Counter object is used to maintain a counter down mechanism for tracking
    the time remaining for an individual timer.

    The Counter interface is NOT inherently thread safe.  
 */
class CounterCallback_ : public Cpl::Container::ExtendedItem
{
public:
    /// Notification of the count reaching zero
    virtual void expired( void ) noexcept = 0;

    /// Decrements the counter
    virtual void decrement( unsigned long milliseconds=1 ) noexcept = 0;

    /// Increments the counter
    virtual void increment( unsigned long milliseconds ) noexcept = 0;

    /// Returns the current count (in milliseconds)
    virtual unsigned long count() const noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~CounterCallback_() {}

};



/** This abstract class defines the interface for registering for
    an Counter object with the Timer Manager
 */
class CounterSource_
{
public:
    /// Register for notification
    virtual void attach( CounterCallback_& clientToCallback ) noexcept = 0;

    /** Unregister for notification.  Returns true if the client was currently
        attached/registered; else false is returned.
     */
    virtual bool detach( CounterCallback_& clientToCallback ) noexcept = 0;

    /** This method converts the requested duration in milliseconds to
        counter ticks.
     */
    virtual unsigned long msecToCounts( unsigned long durationInMsecs ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~CounterSource_() {}

};


};      // end namespaces
};
#endif  // end header latch
