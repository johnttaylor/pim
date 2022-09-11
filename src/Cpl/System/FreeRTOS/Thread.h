#ifndef Cpl_System_FreeRTOS_Thread_h_
#define Cpl_System_FreeRTOS_Thread_h_
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
#include "Cpl/System/Tls.h"
#include "Cpl/Text/FString.h"
#include "task.h"


/** Provides the default stack size (since FreeRTOS does not provide one)
 */
#ifndef OPTION_CPL_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE
#define OPTION_CPL_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE   (1024*3)
#endif


// Forward declaration for making the Tls class a friend
namespace Cpl {
namespace System {
class Tls;
};
};


///
namespace Cpl {
namespace System {
namespace FreeRTOS {

/** This concrete class implements a Thread object using FreeRTOS threads
 */
class Thread : public Cpl::System::Thread
{
protected:
    /// Reference to the runnable object for the thread
    Cpl::System::Runnable&  m_runnable;

    /// ASCII name of the task
    Cpl::Text::FString<configMAX_TASK_NAME_LEN>  m_name;

    /// internal handle
    TaskHandle_t            m_threadHandle;

    /// Thread Local storage
    void*                   m_tlsArray[OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES];


public:
    /** Constructor.
            o Does NOT support the application supplying the stack
              memory. Stack memory is allocated from the HEAP

            o If zero is passed as the stack size, then the default stack size
              is set based on the OPTION_CPL_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE
              parameter.
     */
    Thread( Runnable&      runnable,
            const char*    name,
            int            priority      = CPL_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned       stackSize     = 0
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
    static void entryPoint( void* data );

protected:
    /// Returns access to the TLS key array
    static void** getTlsArray() noexcept;


public:
    /** COMPONENT Scoped constructor to convert the native FreeRTOS thread to a 
        Cpl Thread. THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
     */
    Thread( const char* threadName, Cpl::System::Runnable& dummyRunnable );

    /** This is helper method to 'convert' the first/main FreeRTOS thread
        to a CPL thread.  The method can be called many times - but it
        does the 'conversation' once.  The motivation for this method was
        working with the Arduino platform/framework where it creates
        the first/main FreeRTOS thread.
     */
    static void makeNativeMainThreadACplThread( void );

public:
    /// Housekeeping
    friend class Cpl::System::Thread;
    friend class Cpl::System::Tls;
};

/** This is a helper class that can be used to make the current thread
    a CPL thread.  This class should only be used when the 'application'
    contains active threads there were not created through the CPL
    libraries APIs.  For example: On the Arduino Feather52 platform,
    the Arduino framework creates the 'main' thread.

    ** ONLY USE THIS CLASS IF YOU KNOW WHAT YOU ARE DOING **
 */
class MakeCurrentThreadACplThread : public Cpl::System::Runnable
{
protected:
    // Empty run function
    // Note: Leave my 'running state' set to false -->this is so I don't 
    // terminate the native thread prematurely when/if the Thread instance
    // is deleted.  In theory this can't happen since the Thread and Runnable
    // instance pointers for the native thread are never exposed to the 
    // application and/or explicitly deleted.
    void appRun() {}


public:
    /// Converts the native thread to a CPL thread
    MakeCurrentThreadACplThread( const char* threadName="main" )
    {
        // Create a thread object for the native thread
        m_running = true;
        new Cpl::System::FreeRTOS::Thread( threadName, *this );
    }
};


};      // end namespaces
};
};
#endif  // end header latch
