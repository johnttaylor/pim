#ifndef Cpl_System_EventLoop_h_
#define Cpl_System_EventLoop_h_
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

#include "Cpl/System/Runnable.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/Signable.h"
#include "Cpl/System/EventFlag.h"
#include "Cpl/System/SharedEventHandler.h"
#include "Cpl/System/TimerManager.h"


/** Specifies the default timeout period for waiting on a event.
 */
#ifndef OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD
#define OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD       1  //!< 1 msec timeout, aka 1 msec timer resolution for Software Timers
#endif

 ///
namespace Cpl {
///
namespace System {

/** This concrete class is a Runnable object that provides a event driven
    execution model for a thread.  The thread will remaining blocked until
    an "event" occurs.  The following "events" are supported:

    1) The EventLoop Semaphore was signaled
    2) A Event Flag was set( Cpl::System::EventFlag)
    3) A Software Timer expiring (Cpl::System::Timer)

    To receive a callbacks when Event Flags are set, the application must create
    a child class and provide its own implementation of the processEventFlag()
    method.

    The class also provides a mechanism for registering 'other' events to
    be processed.

    Note: The EventLoop does NOT use/consume the Thread Semaphore.
 */
class EventLoop : public Runnable, public EventFlag, public Signable, public TimerManager
{
public:
    /** Constructor. The 'timeOutPeriodInMsec' parameter specifies how
        long the EventLoop will wait for an event before timing out and
        processing the software timers.  The value of 'timeOutPeriodInMsec' is
        the resolution of the timing source for the Software Timers, i.e. the
        event loop will 'wake-up' at least every 'timeOutPeriodInMsec'
        milliseconds.

        A fatal error is generated if 'timeOutPeriodInMsec' is set to zero.
     */
    EventLoop( unsigned long          timeOutPeriodInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
               SharedEventHandlerApi* eventHandler        = 0 );

    /// Virtual destructor
    virtual ~EventLoop() {};


protected:
    /** This method is used to initialize the Event Loop's thread has started
        to executed.  
        
        This method is intended to be used by child classes that are extending 
        the Event Loop.  For this use case - this method MUST be called once on 
        the beginning of the appRun() method and BEFORE the "main loop" for the 
        appRun() method is entered.
     */
    virtual void startEventLoop() noexcept;

    /** This method is used to wait (and process) the next event(s).  This
        method returns after being woken up and processing all timer expired
        and pending Event Flag events. This method should always be wrapped
        in a loop (typically a forever loop).
        
        The method typically returns true.  The method only returns false if
        the pleaseStop() method was called on the Event Loop instance.

        This method is intended to be used by child classes that are extending 
        the Event Loop.  For this use case - this method MUST be called inside
        the child class's "main loop" in its the appRun() method.

        Example appRun() for a child class extending the Event Loop:
        @code

        void ChildEventLoop::appRun( void )
        {
            startEventLoop();
            <child specific intialization>

            bool run = true;
            while( run )
            {
                run = waitAndProcessEvents();
                if ( run )
                {
                    <child specific event processing>
                }
            }
            stopEventLoop()
        }
    
        @endcode
     */
    virtual bool waitAndProcessEvents() noexcept;

    /** This method is used to clean-up the Event Loop's when the thread is
        being stopped.

        This method is intended to be used by child classes that are extending
        the Event Loop.  For this use case - this method MUST be called once
        AFTER the event-processing loop has exited.
     */
    virtual void stopEventLoop() noexcept;

protected:
    /** This method is used (by the concrete child class(es)) to process one
        or more Event Flags.  This method is called when at least one Event
        Flag was set. The method is called N consecutive times - one call for
        each Event Flag that is set.  The 'eventNumber' (which is zero based)
        identifies which Event Flag is/was set.

        If no ShareEventHandler was provided when the event loop was created,
        the default implementation of this method does NOTHING; else the
        ShareEventHandler instance is used to process the event flag/number.
     */
    virtual void processEventFlag( uint8_t eventNumber ) noexcept;

public:
    /// See Cpl::System::Signable
    int signal( void ) noexcept;

    /// See Cpl::System::Signable
    int su_signal( void ) noexcept;


public:
    /// See Cpl::System::Runnable
    void pleaseStop();


protected:
    /// See Cpl::System::Runnable
    void appRun();

public:
    /// See Cpl::System::EventFlag
    void notifyEvents( Cpl_System_EventFlag_T events ) noexcept;

    /// See Cpl::System::EventFlag
    void notify( uint8_t eventNumber ) noexcept;

    /// See Cpl::System::EventFlag
    void su_notifyEvents( Cpl_System_EventFlag_T events ) noexcept;

    /// See Cpl::System::EventFlag
    void su_notify( uint8_t eventNumber ) noexcept;

public:
    /// See Cpl::System::Runnable
    void setThreadOfExecution_( Thread* myThreadPtr );

protected:
    /// A pointer to the thread the Event Loop executes in
    Thread*                 m_myThreadPtr;

    /// My shared event handler (if I have one)
    SharedEventHandlerApi*  m_eventHandler;
    
    /// Semaphore associated with the mailbox (note: the Thread semaphore is NOT used)
    Cpl::System::Semaphore  m_sema;

    /// Timeout period for waiting on the next event
    unsigned long           m_timeout;

    /// The variable holds the current state of all Event Flags
    Cpl_System_EventFlag_T  m_events;

    /// Flag used to help with the pleaseStop() request
    bool                    m_run;

};

};      // end namespaces
};
#endif  // end header latch
