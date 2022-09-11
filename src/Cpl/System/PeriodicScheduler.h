#ifndef Cpl_System_PeriodicScheduler_h_
#define Cpl_System_PeriodicScheduler_h_
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

#include "Cpl/System/ElapsedTime.h"

/** Helper macro that is used to mark the end of an 'Interval Array'
 */
#define CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS     {nullptr,{0,0},nullptr}


 ///
namespace Cpl {
///
namespace System {

/** This concrete class is a 'policy' object that is used to add polled based,
    cooperative monotonic scheduling to a Runnable object.  Monotonic in this
    content means that if a 'interval' method is scheduled to execute at a
    periodic rate of 200Hz, then the method will be called on every 5ms boundary
    of the system time.

    The scheduler makes its best attempt at being monotonic and deterministic,
    BUT because the scheduling is polled and cooperative (i.e. its is the
    application's responsibility to not-overrun/over-allocate the processing
    done during each interval) the actual the timing cannot be guaranteed.
    That said, the scheduler will detect and report when the interval timing
    slips. 

    The usage of this class is to implement (or extended) a Cpl::System::Runnable
    object, i.e. invoked inside the 'forever' loop of the Runnable object's appRun()
    method.  The scheduler can be use within a 'bare' forever loop and it can be
    used with existing event based Runnable objects to add periodic scheduling.
    See Cpl::Dm::PeriodicScheduler for example extending an event based Runnable
    object to include periodic scheduling.
 */
class PeriodicScheduler
{
public:
    /** Definition for an interval method.  This method is called when the
        period time has expired for the interval.

        Where:
            currentTick     - is current system when the interval method is called
            currentInterval - is the deterministic interval boundary that is being
                              logically executed.
            context         - Application defined argument/value passed to the
                              interval method.

        Example:
            Given a interval method is scheduled to execute at 10Hz and the
            current system time in the Runnable object's forever loop is 10.212
            seconds when the interval method is called THEN:

                currentTick:=      10212 ms
                currentInterval:=  10200 ms
      */
    typedef void (*IntervalCallbackFunc_T)(ElapsedTime::Precision_T currentTick,
                                            ElapsedTime::Precision_T currentInterval,
                                            void* context);

    /** Defines an interval.  The application should treat this data struct as
        an opaque structure.
     */
    struct Interval_T
    {
        IntervalCallbackFunc_T      callbackFunc; //!< Callback function pointer
        void*                       context;      //!< Optional Context for the callback.  The callback function is required to 'understand' the actual type of the context pointer being passed to it.
        ElapsedTime::Precision_T    duration;     //!< The amount time in the Interval's period.
        ElapsedTime::Precision_T    timeMarker;   //!< Internal Use ONLY: Marks the last time the interval executed    

        /// Constructor
        Interval_T( IntervalCallbackFunc_T   callbackFunc,
                    ElapsedTime::Precision_T periodTime,
                    void*                    context  = nullptr )
            : callbackFunc( callbackFunc )
            , context( context )
            , duration( periodTime )
            , timeMarker( { 0,0 } )
        {
        }

        /// Data accessor
        inline IntervalCallbackFunc_T getCallbackFunction() { return callbackFunc; }

        /// Data accessor
        inline ElapsedTime::Precision_T getDuration() { return duration; }

        /// Data accessor
        inline void* getContext() { return context; }
    };


    /** Defines the method that is used to report to the Application when an
        Interval does not execute 'on time'

        Where:
            intervalThatSlipped  - Reference to the interval that did not execute on time
            currentTick          - The system time when the scheduler executed the interval
            missedInterval       - The interval boundary that was missed
            intervalContext      - Application defined argument/value that was
                                   passed to the interval method.

     */
    typedef void (*ReportSlippageFunc_T)(Interval_T& intervalThatSlipped,
                                          ElapsedTime::Precision_T currentTick,
                                          ElapsedTime::Precision_T missedInterval );

    /** Defines the function that returns current system.  This method has two
        purposes:
            1) It simplifies unit testing because it breaks the dependency
               on 'real time
            2) Makes unit testing easier
      */
    typedef ElapsedTime::Precision_T (*NowFunc_T)();

    /** Defines the optional functions that are used to provide hooks during
        startup/shutdown of the thread/loop[ to perform application specific 
        processing. 
      */
    typedef void( *Hook_T )(ElapsedTime::Precision_T currentTick );

public:
    /** Constructor. The application provides a variable length array of interval
        definitions that will be scheduled.  The last entry in the
        array must contain a 'null' Interval_T definition (i.e. all fields
        set to zero).  The Scheduler assumes that each Interval_T definition
        has a unique period time.

        The individual 'intervals' MUST be initialized (either statically or
        by calling initializeInterval()) before creating the scheduler.

        Notes:
            - When extending a event based (i.e. inherits from EventLoop) runnable
              object with scheduling, the Application should define all of the
              scheduled Intervals to have period times that are multiples of
              the EventLoop's 'timeOutPeriodInMsec' constructor value.
     */
    PeriodicScheduler( Interval_T           intervals[],
                       Hook_T               beginThreadProcessing = nullptr,
                       Hook_T               endThreadProcessing   = nullptr,
                       ReportSlippageFunc_T slippageFunc          = nullptr,
                       NowFunc_T            nowFunc               = ElapsedTime::precision );

    /// Virtual destructor
    virtual ~PeriodicScheduler() {};


public:
    /** This method is used to invoke the scheduler.  When called zero or more
        Interval definitions will be executed.  The method returns true if at
        least one Interval was executed.

        If a scheduled Interval does not execute 'on time', then the reportSlippage()
        method will called.  It is the Application's to decide (what if anything)
        is done when there is slippage in the scheduling. The slippage is reported
        AFTER the Interval's IntervalCallbackFunc_T is called.
     */
    virtual bool executeScheduler();


    /** This method is expected to be called ONCE when the 'thread' is started and
        prior to the thread entering its 'forever' loop
     */
    virtual void beginLoop();

    /** This method is expected to be called ONCE when the 'thread' has exited
        its 'forever' loop (but before the thread has actually terminated)
     */
    virtual void endLoop();

protected:
    /** Helper method to Round DOWN to the nearest 'interval' boundary.
        A side effect the rounding-down is the FIRST execution of an interval
        will NOT be accurate (i.e. will be something less than 'periodMs').
     */
    void setTimeMarker( Interval_T& interval, ElapsedTime::Precision_T currentTick ) noexcept;

protected:
    /// List of Intervals Pointers
    Interval_T*             m_intervals;

    /// Report slippage method
    ReportSlippageFunc_T    m_reportSlippage;

    /// Current system callback
    NowFunc_T               m_nowFunc;

    /// Application hook during thread start-up
    Hook_T                  m_beginThreadFunc;

    /// Application hook during thread shutdown
    Hook_T                  m_endThreadFunc;

    /// Flag to managing the 'first' execution
    bool                    m_firstExecution;
};

};      // end namespaces
};
#endif  // end header latch
