#ifndef Cpl_System_RP2040_Thread_h_
#define Cpl_System_RP2040_Thread_h_
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
#include "Cpl/System/Thread.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/Tls.h"
#include "Cpl/Text/FString.h"



/// Size, in bytes (not including the null terminator), of a thread name
#ifndef OPTION_CPL_SYSTEM_RP2040_THREAD_NAME_LEN        
#define OPTION_CPL_SYSTEM_RP2040_THREAD_NAME_LEN        16
#endif


///
namespace Cpl {
///
namespace System {
///
namespace RP2040 {

/** This concrete class implements a Thread object that maps 1 to 1 with
    a processor core on the Raspberry PI RP2040 micro-controller.

    At least one thread must be created and at most only two threads can be
    created. The first thread created executes on core0.  The second thread
    created executes on core1.

    The first thread MUST be created before enableScheduling() is called. The
    second thread that executes on core1 can be created or after enableScheduling()
    is called. The core1 thread can also be destroyed and recreated after
    enableScheduling() is called.

    The name of the threads are hard coded to 'CORE0' and 'CORE1'.

    The 'native' thread handle is the core number (i.e. 0 or 1).

    Threads can ONLY be created using the Cpl::System::Thread::create() method.
    When created threads:
        - Only the 'runnable' and 'name' should be specified.
        - The stack sizes are defined by the build symbols: PICO_STACK_SIZE
          and PICO_CORE1_STACK_SIZE, e.g. -DPICO_STACK_SIZE=4096 -DPICO_CORE1_STACK_SIZE=2048
        - Thread priority has NO meaning since both threads/cores execute concurrently.
        - The thread(s) begin executing after enableScheduling() is called.

    NOTES: 
        1. The threads do NOT 'execute' (i.e. call their Runnable object's run() 
           method till enableScheduling() is called.  In addition the enableScheduling()
           method NEVER returns.
        2. The suspendScheduling() semantics suspends ALL processing on the 'other'
           core.  This includes interrupt processing on the other core.

  */
class Thread : public Cpl::System::Thread
{
protected:
    /// Private Constructor. Application can only create thread using the Cpl::System::Thread::create() method.
    Thread( Runnable& runnable, const char* name, unsigned coreId );

public:
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



public:
    /** COMPONENT SCOPED METHOD.  The application should NEVER call this
        this method.

        Helper method to internally allocate a thread. 
     */
    static void createThreadInstance( unsigned coreId, Cpl::System::Runnable& runnable, const char* name ) noexcept;


protected:
    /// The thread synchronized message semaphore.
    Cpl::System::Semaphore  m_syncSema;

    /// Pointer to the runnable object for the thread
    Cpl::System::Runnable*  m_runnable;

    /// Thread name
    Cpl::Text::FString<OPTION_CPL_SYSTEM_RP2040_THREAD_NAME_LEN> m_name;

    /// internal handle
    unsigned                m_coreId;

public:
    /// Housekeeping
    friend class Cpl::System::Thread;
};


};      // end namespaces
};
};
#endif  // end header latch
