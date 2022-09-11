#ifndef Cpl_System_Win32_Thread_h_
#define Cpl_System_Win32_Thread_h_
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

#include "Cpl/System/Thread.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/Text/FString.h"


///
namespace Cpl {
///
namespace System {
///
namespace Win32 {

/** This concrete class implements a Thread object using Win32 threads
    NOTE: The class/implementation uses _beginthread() instead of
          CreateThread() ->this is per Microsoft's documentation
          that says the _beginthread() properly initializes/cleans-up
          the C-Runtime library as where CreateThread() does NOT.
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

    /// internal handle
    HANDLE                  m_threadHandle;

    /// Priority
    int                     m_priority;

    /// Option to allow simulate ticks
    bool                    m_allowSimTicks;

    /// The thread synchronized message semaphore.
    Cpl::System::Semaphore  m_syncSema;


public:
    /** Constructor.  Priority is 0=highest, 30=lowest, normal=15.
        NOTE: Does NOT support the application supplying the stack
              memory.
     */
    Thread( Runnable&   runnable,
            const char* name,
            int         priority      = CPL_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned    stackSize     = 0,
            bool        allowSimTicks = true
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
    static void __cdecl entryPoint( void* data );


public:
    /** COMPONENT Scoped constructor to convert the native Win32 thread to a 
        Cpl Thread. THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
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
