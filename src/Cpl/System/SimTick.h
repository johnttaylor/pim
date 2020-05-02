#ifndef Cpl_System_SimTick_h_
#define Cpl_System_SimTick_h_
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


#include <stddef.h>
#include "colony_config.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/Container/Item.h"


/** The amount of (real) time, in milliseconds, for the simulated-tick engine to
    wait for at least one response from a simulated-tick-thread before aborting
    an advance() command (i.e. declaring all threads terminated OR deadlocked)
 */
#ifndef OPTION_CPL_SYSTEM_SIM_TICK_NO_ACTIVITY_LIMIT
#define OPTION_CPL_SYSTEM_SIM_TICK_NO_ACTIVITY_LIMIT    (2*1000)
#endif

/** Minimum number of ticks required to advance the simulated time.  This
    parameter is a work-around for a non-Real Time OS (such as Windoze) which
    can not actually sleep/wait for 1msec.  The parameter is defaulted to
    10, i.e. after the simulation has been advance 10 tick -->the simulated 
    time will be advanced 10msec.

    NOTE: The parameter also allows the simulation to be "speed-up" if the 
          timing requirements of the application under test do not require
          millisecond resolution, e.g. setting this value to 1000 (aka 1 second)
          the simulation runs MUCH faster than real time
 */
#ifndef OPTION_CPL_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE
#define OPTION_CPL_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE    10
#endif

/** Value passed to the Cpl::System::sleep() call to effectively yield the CPU
    do allow the threads running in simulated time to actually begin executing
 */
#ifndef OPTION_CPL_SYSTEM_SIM_TICK_YEILD_SLEEP_TIME
#define OPTION_CPL_SYSTEM_SIM_TICK_YEILD_SLEEP_TIME         1
#endif

#ifdef USE_CPL_SYSTEM_SIM_TICK
/** This macro is a wrapper for the topLevelWait() call. The macro allows the
      call to be compiled-out when simulate time is NOT enabled.
   */
#define CPL_SYSTEM_SIM_TICK_TOP_LEVEL_WAIT()            Cpl::System::SimTick::topLevelWait()

/** This macro is a wrapper for the applicationWait() call.  The macro allows the
    call to be compiled-out when simulated time is NOT enabled
 */
#define CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT()          Cpl::System::SimTick::applicationWait()


/** This macro is a wrapper for the usingSimTicks() call. The macro allows the call
        to be compiled-out when simulate time is NOT enabled.
  */
#define CPL_SYSTEM_SIM_TICK_USING_SIM_TICKS()           Cpl::System::SimTick::usingSimTicks()


 /** This COMPONENT Scoped macro is a wrapper for threadInit_() call.  The macro allows the
     call to be compiled-out when simulate time is NOT enabled. NOTE: The
     application SHOULD NEVER use this macro
  */
#define CPL_SYSTEM_SIM_TICK_THREAD_INIT_(f)             Cpl::System::SimTick::threadInit_(f)


/** This COMPONENT Scoped macro is a wrapper for onThreadExit_() call.  The macro allows the
    call to be compiled-out when simulate time is NOT enabled. NOTE: The
    application SHOULD NEVER use this macro
 */
#define CPL_SYSTEM_SIM_TICK_ON_THREAD_EXIT_()           Cpl::System::SimTick::onThreadExit_()



#else
/// Simulate Tick disabled
#define CPL_SYSTEM_SIM_TICK_TOP_LEVEL_WAIT()

/// Simulate Tick disabled
#define CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT()

/// Simulate Tick disabled
#define CPL_SYSTEM_SIM_TICK_USING_SIM_TICKS()           false

/// Simulate Tick disabled
#define CPL_SYSTEM_SIM_TICK_THREAD_INIT_(f)

/// Simulate Tick disabled
#define CPL_SYSTEM_SIM_TICK_REAL_TIME_THREAD_INIT_()  

/// Simulate Tick disabled
#define CPL_SYSTEM_SIM_TICK_ON_THREAD_EXIT_()

#endif





/// 
namespace Cpl {
/// 
namespace System {


/** This class define the interface to provide a simulates system tick (in
    milliseconds) to the application. The goal is provide a mechanism for
    an application to run in 'simulate time' instead of real time. The
    simulated tick mechanism is for the most part transparent an application,
    i.e. the application programmer does NOT have to be aware or design
    his code differently to be able to run in simulate tick mode.  The
    'intrusions points' are detailed below. The typical uses cases for using
    simulated time are:

@htmlonly
<pre>

        o Testing.  A simulated system tick allows the test harness to have
          deterministic control of the application's time.  In addition, time
          can be singled stepped, paused, accelerated (i.e. run N days in
          M minutes).

        o Model in the Loop (MIL).  An external application such as Matlab/
          Simulink can be used to provide the simulated tick input as well
          as capture the application's output and provide the application
          inputs, i.e. connecting the application to a simulated physical
          world.  Note: 'connecting' a MIL application to a simulation such
          as Matlib/Simulink requires a communication layer that is NOT
          provided by Colony as well as specialization to Matlib/Simulink
          that mates up with the afore mentioned communication layer.  This
          is left as an exercise to the reader :).


    NOTES:
    ------
        o All timing sources, e.g. Cpl::Api::sleep(), provided by Colony.Core
          have support for simulated ticks build in.

        o All of the Colony.Core's ITC message queues/mailbox servers, etc. also
          have support for simulated ticks build in.

        o With respect to threads that are using simulated time, There needs to
          be at least one thread (at any given time) that is a 'timing' source,
          e.g. waiting on sleep(), timedWait(), etc.  If your application uses
          a Cpl::System::EventLoop (or a sub-class) instance - then this 
          requirement has been met. Worse case you a create a thread that 
          forever loops on sleep().

        o This interface is independent of the underlaying platform, i.e works
          on ALL platforms.

        o NOT all threads and/or parts of an application need or should use
          simulated time.  For example the test harness (or communication
          interface to a simulation tool) that provide the stimulus to
          advance the simulated tick need to run in real-time.  It is the
          Application responsibility to properly selected what is and is
          not using simulated time.

            - The platform's native/main thread ALWAYS runs in 'real-time', i.e.
              call to SimTick::wait() never blocks.  Also any Api::sleep()
              calls from the native/main thread are redirected to the
              Api::sleepInRealTime().

        o THREADING: This interface provides ONLY a functional simulation of
          system ticks. These means the actual runtime timing of threads and/or
          sequence of execution will NOT be the same as the 'live'
          non-simulated application.

            - When using simulated ticks with multi-threaded applications, the
              timing of the threads using the simulated ticks behavior is 'better'
              if the thread supplying the ticks has LOWER priority than the
              threads using the simulate ticks.

            - BEWARE of multi-core targets.  Because many threads can actually
              execute at the SAME time on multi-core boxes, the timing between
              the tick source thread and the simulated-tick threads and between
              simulated-tick threads is UNPREDICTABLE.  The tick sequencing
              is guaranteed, but the order within a tick OR timing relations to
              the tick source OR inter-tick timing is NOT guaranteed.

            - Not all use cases using a simulated tick and threading will work
              and/or are supported.  The simulated tick logic only works for
              event driven and/or frequency based where all actions for a time
              period as completed in on pass of the "main loop".  Other issues:
                
                o The assumption is that all calls to a mutex will cause the 
                  thread to block across simulated ticks, i.e. mutex can ALWAYS 
                  be successfully acquired and release within the context of 
                  single simulated tick

            - Be careful about deleting threads when using Simulated Ticks -
              brute force terminating threads in this scenario has unpredictable
              and/or improper behavior. Always have threads run to completion
              when using Simulate Ticks.

        o The DEFAULT implementation of this interface assumes that one
          system tick EQUALS one millisecond.


    HOW TO ADD SIMULATED TICK SUPPORT
    ---------------------------------
    o Enabling Simulated time is done at compile done.  The following steps
      are required:
        1) Compile with the preprocessor symbol USE_CPL_SYSTEM_SIM_TICK
           defined.

        2) Compile & Link the src/Cpl/System/_simtick directory.  Also do NOT
           compile/link the src/Cpl/System/xxx/_realtime directories

    o Typically thats all that needs to be done. The only additional work is if
      application creates thread(s) that have an application specific (i.e. not
      a Colony.Core Event Loop, mailbox server, etc.) 'forever' loop AND it
      is desired for the thread(s) to use 'simulated time'.  For this scenario
      the application must add a call to this interface at the top of the
      forever loop.  See the example below:

        void myThreadEntryFunction( void* arg )
            {
            ...

            while(1)
                {
                // Block the main loop until the next simulated system tick
                CPL_SYSTEM_SIM_TICK_TOP_LEVEL_WAIT();
                ...
                }
            }
         }

</pre>
@endhtmlonly
 */
class SimTick : public Cpl::Container::Item
{
public:
    /** This method will cause the current thread to block UNTIL one
        simulated system tick has elapsed.  Every call to this method
        consumes one simulated system tick. This method SHOULD only be used
        at the very most top level of a thread's 'forever' loop.  

        This method SHOULD NEVER be called directly, but via the preprocessor
        macro CPL_SYSTEM_SIM_TICK_TOP_LEVEL_WAIT().  This allows the simulate
        tick code to be compiled out of production versions of the application.
     */
    static void topLevelWait( void ) noexcept;

    /** This method returns true if the current thread is using 'simulated time'

        This method SHOULD NEVER called directly, but via the preprocessor
        macro CPL_SYSTEM_SIM_TICK_USING_SIM_TICKS().  This allows the simulate tick
        code to be compiled out of production versions of the application.
     */
    static bool usingSimTicks( void ) noexcept;

    /** This method is used to support blocking calls (such as wait on semaphore,
        sleep(), etc.) within an individual iteration of thread's top level
        loop. The 'block' in this case IS NOT waiting on the simulated tick -->
        it is waiting on the application logic.

        This method SHOULD NEVER called directly, but via the preprocessor
        macro CPL_SYSTEM_SIM_TICK_APPLICATION_WAIT().  This allows the simulate 
        tick code to be compiled out of production versions of the application.
     */ 
    static void applicationWait( void ) noexcept;

    /** This method returns the current simulated tick count
     */
    static size_t current( void ) noexcept;


public:
    /** Calling this method advances the application's time by 'numTicks'. This
        method should ONLY called by a test harness or some other module that
        is typically not part of the application, i.e. the caller of this
        method must be 'executing' in real time.  Internally the simulated
        time is advance at one tick at time.

        The method returns true after completing advancing the internal tick
        count by the amount specified.  False is only returned when the
        simulate tick engine has detected that there a NO simulated-tick-threads
        executing OR that ALL simulated-tick-threads are dead locked.
     */
    static bool advance( size_t numTicks ) noexcept;


    /** This method returns true if the specified thread (by Thread ID) is
        blocked waiting on the next simulate tick.  While this method is
        thread safe -->it is intended (and only make sense) to be called
        from the SAME thread that also calls the advance() method.
     */
    static bool isWaitingOnNextTick( size_t threadID ) noexcept;


public:
    /** This COMPONENT Scoped method is used during thread creation to insert the
        necessary hooks (per thread) for the simulate tick engine.  If
        'useSimTicks' is false then thread does NOT use simulated time. This
        method SHOULD NEVER be called by the application.
     */
    static void threadInit_( bool useSimTicks=true ) noexcept;

    /** This COMPONENT Scoped method is used during thread deletion to insert the
        necessary hooks (per thread) for the simulate tick engine. This
        method SHOULD NEVER be called by the application.
     */
    static void onThreadExit_( void ) noexcept;


protected:
    /// The thread's current simulated time in ticks
    size_t    m_curTicks;

    /// Semaphore used to wait on a simulated tick
    Semaphore m_waiter;

    /// Flag that keeps track if I need to signal/ack-back to the tick source for the current tick
    bool      m_ackPending;

    /// Thread ID of the thread using the simulate tick
    size_t    m_threadId;


protected:
    /// Constructor
    SimTick();

    /// Helper method. Returns true if the thread was queued for the next simulated tick
    static bool testAndQueue( SimTick* simInfoPtr ) noexcept;

    /// Helper method. 
    static unsigned wakeUpWaiters( void ) noexcept;

    /// Helper method. 
    static unsigned getCurrentWaitersCount( void ) noexcept;

    /// Friend(s)
    friend class ElapsedTime;
    friend class Api;
};


};      // end namespaces
};
#endif  // end header latch



