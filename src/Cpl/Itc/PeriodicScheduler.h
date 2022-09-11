#ifndef Cpl_Its_PeriodicScheduler_h_
#define Cpl_Its_PeriodicScheduler_h_
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

#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/System/PeriodicScheduler.h"


///
namespace Cpl {
///
namespace Itc {

/** This class extends the Cpl::Itc::MailboxServer class to add periodic scheduling 
    to an event based 'thread'.  The timing resolution of the periodic scheduling 
    is a determined by the 'timingTickInMsec' argument value in the class's 
    constructor.  For example if 'timingTickInMsec' is 10ms then no interval 
    should have a interval duration less than 10ms.

    The order of processing is:

       1. Event Flags are processed.  Events are processed in LSb order.
       2. The timers and their callbacks (if any timers have expired) are
          processed.
       3. The Periodic scheduler's executeScheduler() method is called
       4. A single ITC message (it there is on pending) is dispatched.
       5. The optional 'Idle' function is called.
       6. The loop is repeated until there are no expired timers, no event
          flags, no MP change notifications, and no ITC messages - at which 
          point the thread blocks and wait for any of the above asynchronous 
          actions to wake up the thread.

 */
class PeriodicScheduler : public Cpl::Itc::MailboxServer, public Cpl::System::PeriodicScheduler
{
public:
    /** Defines an optional method that is called every time the Runnable's
        object executes its event/scheduling loop
     */
    typedef void (*IdleFunc_T)(Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted);


public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers AND for the
        periodic scheduling.
     */
    PeriodicScheduler( Interval_T                          intervals[],
                       Hook_T                              beginThreadProcessing = nullptr,
                       Hook_T                              endThreadProcessing   = nullptr,
                       ReportSlippageFunc_T                slippageFunc          = nullptr,
                       NowFunc_T                           nowFunc               = Cpl::System::ElapsedTime::precision,
                       IdleFunc_T                          idleFunc              = nullptr,
                       unsigned long                       timingTickInMsec      = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                       Cpl::System::SharedEventHandlerApi* eventHandler          = 0 ) noexcept;

public:
    /// See Cpl::System::Runnable
    void appRun();


protected:
    /// Cache the Idle function pointer
    IdleFunc_T m_idleFunc;
};

};      // end namespaces
};
#endif  // end header latch