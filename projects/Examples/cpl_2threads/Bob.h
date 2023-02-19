#ifndef Bob_h
#define Bob_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file defines the Module Bob that is silly module that blinks a LED
    at specified frequency.  Other modules interact with the Bob module via
    model points.
 */

#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/System/ElapsedTime.h"


 /** This class blinks a LED at frequency specified by the 'delayTime' model 
     point provided in its constructor.  The frequency will be 2 x 'timing'.  
     The use of the model points provides a thread safe mechanism for other 
     modules to interact with Bob.

     The class uses the model point 'verbose' to enable/disable its text output.
     The text output uses the Trace statement to provide thread information and
     time stamps for the output messages.

     The class uses model point change notifications to set at runtime its 
     delay time and verbose state.  Specifically, the developer uses the 
     Console TShell command 'dm' to modify the model points and the Bob object 
     updates it configuration based on the model point change notifications 
     (i.e. event driven callbacks).

     The class uses 'periodic scheduling' for the LED timing.  The timing could
     have been with a Software Timer (i.e. a callback to the Bob class) - but 
     Periodic Scheduling is used to illustrate both Periodic Scheduling and
     events handling in a single thread.

     At run time - the application is required to call start() and stop() on 
     the module instance to start and stop the instance. The start() and stop()
     MUST be called AFTER scheduling has been enabled.  This is because the 
     class's model point change notification subscriptions MUST be done in the 
     thread context that the callback functions will occur in.
  */
class Bob
{
public:
    ///
    Bob( Cpl::Dm::MailboxServer&  myMbox,
         Cpl::Dm::Mp::Uint32&     delayTimeModelPoint,
         Cpl::Dm::Mp::Bool&       verboseModelPoint );

public:
    /// In-thread initialization of the module
    void start();

    /// In-thread shutdown of the module
    void stop();

protected:
    /// This method is called when the Model point changes state/value
    void delayModelPointChange( Cpl::Dm::Mp::Uint32 & mp );

    /// This method is called when the Model point changes state/value
    void verboseModelPointChange( Cpl::Dm::Mp::Bool & mp );

public:
    /// This method is called from the Periodic Scheduler
    static void periodicInterval( Cpl::System::ElapsedTime::Precision_T currentTick,
                                  Cpl::System::ElapsedTime::Precision_T currentInterval,
                                  void*                                 context );

protected:
    /// Reference to my model point
    Cpl::Dm::Mp::Uint32&     m_mpDelayTime;

    /// Reference to my model point
    Cpl::Dm::Mp::Bool&       m_mpVerbose;

    /// Observer for getting change notifications
    Cpl::Dm::SubscriberComposer<Bob, Cpl::Dm::Mp::Uint32>   m_observerDelay;

    /// Observer for getting change notifications
    Cpl::Dm::SubscriberComposer<Bob, Cpl::Dm::Mp::Bool>     m_observerVerbose;

    /// Time, in milliseconds, to delay between toggling the LED
    Cpl::System::ElapsedTime::Precision_T                   m_delayMs;

    /// Time, in milliseconds, of when the LED was last toggled
    Cpl::System::ElapsedTime::Precision_T                   m_timeMarker;

    /// Free running counter that track the number of 'edges'
    uint32_t        m_numEdges;

    /// Verbose flag
    bool            m_verbose;

    /// Keep track of my open/close state
    bool            m_opened;

};


#endif  // end header latch
