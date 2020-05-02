#ifndef Cpl_System_Timer_h_
#define Cpl_System_Timer_h_
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

#include "Cpl/System/TimerManager.h"
#include "Cpl/System/Counter_.h"


///
namespace Cpl {
///
namespace System {

/** This mostly concrete interface defines the operations that can be performed
    on a software timer.  Software timers execute 'in-thread' in that all
    operations (start, timer expired callbacks, etc.) are performed in a single
    thread.

    Because the timer context (i.e. the timer owner), timer methods and
    callbacks all occur in the same thread, the timer context will never
    receive a timer expired callback AFTER the timer's stop() method has been
    called.

    NOTES:
        o The timer context must implement the following method:
                virtual void expired( void ) noexcept;

        o Because the timing source of an individual thread may NOT be a
          clean divider of the timer duration, the timer duration is taken
          as the minimum.  For example: if the timing source has a resolution
          of 20msec per count, and the timer duration on the start() timer
          call is 5 msec, then the timer will expire after the next full count,
          i.e. after 20msec, not 5msec.  IT IS THE APPLICATION'S RESPONSIBILITY
          TO MANAGE THE RESOLUTION OF THE TIMING SOURCES.
 */
class Timer : public CounterCallback_
{
protected:
    /// The timer's tick source
    TimerManager* m_timingSource;
    
    /// Current count
    unsigned long           m_count;

public:
    /// Constructor    
    Timer( TimerManager& timingSource );

    /// Constructor. Alternate constructor - that defers the assignment of the timing source    
    Timer();

public:
    /** Starts the timer with an initial count down count duration of
        'timerDurationInMilliseconds'.  If the timer is currently running, the
        timer is first stopped, and then restarted.
     */
    virtual void start( unsigned long timerDurationInMilliseconds ) noexcept;

    /** Stops the timer. It is okay to call stop() even after the timer has
        previously expired or explicitly stopped.
     */
    virtual void stop() noexcept;

public:
    /** Sets the timing source.  This method CAN ONLY BE CALLED when the
        timer is has never been started or it has been stopped
     */
    virtual void setTimingSource( TimerManager& timingSource ) noexcept;

protected:  // CounterCallback_ API
    /// See Cpl::System::CounterCallback_
    void decrement( unsigned long milliseconds=1 ) noexcept;

    /// See Cpl::System::CounterCallback_
    void increment( unsigned long milliseconds ) noexcept;

    /// See Cpl::System::CounterCallback_
    unsigned long count() const noexcept;
};


/** This template class implements a Software Timer that is context
    independent and allows for a single context to contain many Timers.

    Template args:
        CONTEXT   Type of the Class that implements the context for the timer
  */

template <class CONTEXT>
class TimerComposer : public Timer
{
public:
    /** Definition of the call-back method that notifies the
        context/client when the timer expires.
     */
    typedef void (CONTEXT::* TimerExpiredFunction_T)();

private:
    ///
    CONTEXT & m_context;
    ///
    TimerExpiredFunction_T  m_expiredFuncPtr;


public:
    /// Constructor
    TimerComposer( TimerManager&          timingSource,
                   CONTEXT&               timerContextInstance,
                   TimerExpiredFunction_T expiredCallbackFunc
    );

protected:
    /// See Cpl::System::CounterCallback_
    void expired() noexcept;

};


//==========================================================================
//                          IMPLEMENTATION
//==========================================================================

/////////////////////
template <class CONTEXT>
TimerComposer<CONTEXT>::TimerComposer
(
    TimerManager&           timingSource,
    CONTEXT&                context,
    TimerExpiredFunction_T  expiredCallback
)
    :Timer( timingSource )
    , m_context( context )
    , m_expiredFuncPtr( expiredCallback )
{
}

template <class CONTEXT>
void TimerComposer<CONTEXT>::expired( void ) noexcept
{
    (m_context.*m_expiredFuncPtr)();
}


};      // end namespaces
};
#endif  // end header latch
