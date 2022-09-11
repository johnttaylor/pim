#ifndef Cpl_System_TimerManager_h_
#define Cpl_System_TimerManager_h_
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

#include "Cpl/System/Counter_.h"
#include "Cpl/Container/DList.h"


///
namespace Cpl {
///
namespace System {

/** This mostly concrete class implements manages a list of Software Timers.
    When an individual Timer object's count reaches zero, its callback method
    is invoked and it is remove from the Active List.  The Active List is
    designed to be very efficient with respect to decrement counts when there
    are many Timers in the list, i.e. the amount of time to decrement the
    individual Timers is NOT a function of the number of active Timers.

    The Timer Manager requires that the Timer Manager instances, all Timer
    instances, add the Timer's Context (i.e. the code that executes the
    timer expired callbacks) all execute in the SAME thread.
 */
class TimerManager : public CounterSource_
{
public:
    /// Constructor
    TimerManager();


public:
    /** This method starts the Timer Manager.  This method should be called
        only once just before the Main/Event loop processing begins.

        This method must be called from the same thread that the Timer Manager
        executes in.
     */
    void startManager( void ) noexcept;

    /** This method processes the current active timer lists.  For each timer
        that has expired, the timer's context callback method is called.
     */
    void processTimers( void ) noexcept;

    /// Returns true if there are NO active timers
    bool areActiveTimers( void ) noexcept;


public:
    ///  See Cpl::System::CounterCallback_
    void attach( CounterCallback_& clientToCallback ) noexcept;

    ///  See Cpl::System::CounterCallback_
    bool detach( CounterCallback_& clientToCallback ) noexcept;

    ///  See Cpl::System::CounterCallback_
    unsigned long msecToCounts( unsigned long milliseconds ) noexcept;

protected:
    /// Helper method.
    void addToActiveList( CounterCallback_& clientToCallback ) noexcept;

    /** This method is intended to be call by a the timing source and each
        call to this method represents that one tick has expired, i.e. decrement
        the active Counter objects' by one.
     */
    virtual void tick( unsigned long milliseconds=1 ) noexcept;

    /** This method is used by the Tick source to information the Timer Manager
        that there are no more ticks for the timing source's current tick cycle
     */
    virtual void tickComplete( void ) noexcept;

protected:
    /// List of active counters
    Cpl::Container::DList<CounterCallback_> m_counters;

    /// List of Pending-to-attach counters (this happens when timers attach from the timer-expired-callbacks)
    Cpl::Container::DList<CounterCallback_> m_pendingAttach;

    /// Elapsed time of the previous processing cycle
    unsigned long                           m_timeMark;

    /// Elapsed time of the current processing cycle
    unsigned long                           m_timeNow;

    /// Flag to tracks when I am actively processing/consuming ticks
    bool                                    m_inTickCall;
};


};      // end namespaces
};
#endif  // end header latch
