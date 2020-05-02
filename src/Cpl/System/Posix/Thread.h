#ifndef Cpl_System_Posix_Thread_h_
#define Cpl_System_Posix_Thread_h_
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

#include "Cpl/System/Thread.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/Text/FString.h"
#include <pthread.h>

///
namespace Cpl {
namespace System {
namespace Posix {

/** This concrete class implements a Thread object using Posix threads
 */
class Thread : public Cpl::System::Thread
{
protected:
    /// Reference to the runnable object for the thread
    Cpl::System::Runnable&  m_runnable;

    /// ASCII name of the task
    Cpl::Text::FString<64>  m_name;

    /// Thread ID
    size_t                  m_threadID;

    /// Option to allow simulated ticks
    bool                    m_allowSimTicks;

    /// internal handle
    pthread_t               m_threadHandle;

    /// The thread synchronized message semaphore.
    Cpl::System::Semaphore  m_syncSema;


public:
    /** Constructor.

            o POSIX does not define/require specific Priority values, however
              it does define that the priority range must be at 32 and the
              a higher numerical value has higher priority... So the priority
              values support ONLY a range of 32, with N+1 having a higher
              priority than N.  At run time, the HIGHEST/LOWEST bounds will be
              mapped the actual range defined by the functions:
                   sched_get_priority_max(), sched_get_priority_min()

            o The preferred schedType is SCHED_RR or SCHED_FIFO - BUT these
              type require superuser privileges to work.  All of us mere
              mortals must use SCHED_OTHER.  SCHED_OTHER is defined as
              "normal scheduling" - what ever that means.  The biggest problem
              with SCHED_OTHER is that only ONE priority is supported -->BUMMER!
              You can still pass different priority values with SCHED_OTHER -
              but they will have no effect. The priority values WILL work if
              SCHED_RR or SCHED_FIFO is specified.

            o Does NOT support the application supplying the stack
              memory.
     */
    Thread( Runnable&      runnable,
            const char*    name,
            int            priority      = CPL_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned       stackSize     = 0,
            int            schedType     = SCHED_OTHER,
            bool           allowSimTicks = true
    );

    /// Destructor
    ~Thread();

public:
    /// See Cpl::System::Thread
    const char* getName() noexcept;

    /// See Cpl::System::Thread
    size_t getId() noexcept;

    /// See Cpl::System::Thread
    bool isRunning( void ) noexcept;

    /// See Cpl::System::Thread
    Cpl_System_Thread_NativeHdl_T getNativeHandle( void ) noexcept;

    /// See Cpl::System::Thread
    Runnable& getRunnable( void ) noexcept;

public:
    /// See Cpl::System::Signable
    int signal( void ) noexcept;

    /// See Cpl::System::Signable
    int su_signal( void ) noexcept;



private:
    /// Entry point for all newly created threads
    static void* entryPoint( void* data );


public:
    /** Private constructor to convert the native Posix thread to a Cpl Thread.
        THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
     */
    Thread( Cpl::System::Runnable& dummyRunnable );


public:
    /// Housekeeping
    friend class Cpl::System::Thread;
};


};      // end namespaces
};
};
#endif  // end header latch
