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

#include "Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Container/SList.h"


//
using namespace Cpl::System::FreeRTOS;


// Initialize static class variables
static Cpl::Container::SList<Thread>  	threadList_( "StaticConstructor" );
static const char*                      emptyString_ = "";

static void addThreadToActiveList_( Thread& thread );
static void removeThreadFromActiveList_( Thread& thread );


#define AREA_	"Cpl::System::FreeRTOS::Thread"


////////////////////////////////////
#ifdef USE_CPL_SYSTEM_FREERTOS_NATIVE_THREAD 
namespace {

/// This class is used to turn the entry/native/main thread into a Cpl::System::Thread (i.e. add the thread semaphore)    
class RegisterInitHandler_ : public Cpl::System::StartupHook_,
    public Cpl::System::Runnable
{
protected:
    // Empty 'run' function -- it is never called!
    void appRun( void ) {}

public:
    ///
    RegisterInitHandler_() :StartupHook_( eSYSTEM ) {}


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        // Create a thread object for the native thread
        m_running = true;
        new Cpl::System::FreeRTOS::Thread( "main", *this );
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;
#endif

extern bool cpl_system_thread_freertos_schedulerStarted_; // THIS IS BLATANT HACK to work around that xTaskGetSchedulerState() does not work as expected.  This variable is defined in Api.cpp

void Thread::makeNativeMainThreadACplThread( void )
{
    static bool once=false;
    if ( !once )
    {
        once                                         = true;
        cpl_system_thread_freertos_schedulerStarted_ = true;
        new Cpl::System::FreeRTOS::MakeCurrentThreadACplThread();
        CPL_SYSTEM_TRACE_MSG( AREA_, ( "loop(): Main thread has be converted to a CPL thread - I can know use Tracing safely!" ) );
    }
}

////////////////////////////////////
Thread::Thread( const char* threadName, Cpl::System::Runnable& dummyRunnable )
    :m_runnable( dummyRunnable ),
    m_name( threadName ),
    m_threadHandle( xTaskGetCurrentTaskHandle() )
{
    // Initialize by TLS storage for this thread
    for ( unsigned i=0; i < OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES; i++ )
    {
        m_tlsArray[i]   = 0;
    }

    // Plant the address of my TLS array into FreeRTOS's TCB
    vTaskSetApplicationTaskTag( m_threadHandle, (TaskHookFunction_t) this );

    // Add the native thread to the list of active threads
    addThreadToActiveList_( *this );
}


Thread::Thread( Cpl::System::Runnable&   runnable,
                const char*              name,
                int                      priority,
                unsigned                 stackSize
)
    :m_runnable( runnable ),
    m_name( name ),
    m_threadHandle( NULL )
{
    // Initialize by TLS storage for this thread
    for ( unsigned i=0; i < OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES; i++ )
    {
        m_tlsArray[i]   = 0;
    }

    // Calculate stack size in terms of 'depth' (not bytes)
    if ( stackSize == 0 )
    {
        stackSize = OPTION_CPL_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE >> ( sizeof( StackType_t ) / 2 );
    }
    else
    {
        stackSize = stackSize >> ( sizeof( StackType_t ) / 2 );
    }


    // Create the thread
    xTaskCreate( &entryPoint, name, stackSize, this, priority, &m_threadHandle );
    if ( m_threadHandle == NULL )
    {
        Cpl::System::FatalError::logf( "FreeRTOS::Thread::Thread().  Failed to create thread %s.", name );
    }
}

Thread::~Thread()
{
    // NOTE: In general it is not a good thing to "kill" threads - but to 
    //       let the thread "run-to-completion", i.e. have the run() method
    //       of the associated Runnable object complete.  If you do
    //       need to kill a thread - be dang sure that it is state such
    //       that it is ok to die - i.e. it has released all of its acquired
    //       resources: mutexes, semaphores, file handles, etc.
    if ( m_runnable.isRunning() )
    {
        // Ask the runnable object nicely to stop 
        m_runnable.pleaseStop();
        Cpl::System::Api::sleep( 100 );     // Yield execution and allow time for the thread to actually exit.

        // Just to make sure: Brute the force the thread to end - IF it is still running
        // NOTE: This will NOT free any resources associated with the thread including the stack!
        if ( m_runnable.isRunning() )
        {
            vTaskDelete( m_threadHandle );
        }
    }
}


//////////////////////////////
int Thread::signal() noexcept
{
    xTaskNotifyGive( m_threadHandle );
    return 0; // Always return success
}


/** NOTE: This method returns the 'higherPriorityTaskWoken' instead of the
          defined 'return zero on success' semantics.  This is to overcome
          the oddities of FreeRTOS.
 */
int Thread::su_signal() noexcept
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR( m_threadHandle, &higherPriorityTaskWoken );
    return higherPriorityTaskWoken;
}



const char* Thread::getName() noexcept
{
    return m_name;
}

size_t Thread::getId() noexcept
{
    return (size_t) m_threadHandle;
}

bool Thread::isRunning() noexcept
{
    return m_runnable.isRunning();
}

Cpl_System_Thread_NativeHdl_T Thread::getNativeHandle( void ) noexcept
{
    return m_threadHandle;
}

Cpl::System::Runnable& Thread::getRunnable( void ) noexcept
{
    return m_runnable;
}

//////////////////////////////
void Thread::entryPoint( void* data )
{
    // Convert data arg to a pointer to a Thread Object
    Thread* myThreadPtr = (Thread*) data;

    // Plant the address of my TLS array into FreeRTOS's TCB
    vTaskSetApplicationTaskTag( myThreadPtr->m_threadHandle, (TaskHookFunction_t) myThreadPtr );


    // Go Execute the "Runnable" object
    addThreadToActiveList_( *myThreadPtr );
    myThreadPtr->m_runnable.setThreadOfExecution_( myThreadPtr );
    myThreadPtr->m_runnable.run();

    // Remove the thread from the list of active threads
    removeThreadFromActiveList_( *myThreadPtr );

    // Terminate myself
    vTaskDelete( NULL );

    // Per the FreeRTOS API -->this function can NEVER return
    for ( ;;);
}


//////////////////////////////
Cpl::System::Thread& Cpl::System::Thread::getCurrent() noexcept
{
    Thread* curThread = tryGetCurrent();
   
    // Trap potential error
    if ( !curThread )
    {
        Cpl::System::FatalError::logRaw( "FreeRTOS::Thread::getCurrent().  Current thread is NOT a 'Cpl::System::Thread'." );
    }

    return *curThread;
}

Cpl::System::Thread* Cpl::System::Thread::tryGetCurrent() noexcept
{
    TaskHandle_t taskHdl = xTaskGetCurrentTaskHandle();
    if ( taskHdl == 0 )
    {
        return nullptr;
    }
    return (Thread*) xTaskGetApplicationTaskTag( taskHdl );
}

void Cpl::System::Thread::wait() noexcept
{
    ulTaskNotifyTake( pdFALSE, portMAX_DELAY );
}

bool Cpl::System::Thread::tryWait() noexcept
{
    return ulTaskNotifyTake( pdFALSE, 0 ) > 0;
}

bool Cpl::System::Thread::timedWait( unsigned long msecs ) noexcept
{
    return ulTaskNotifyTake( pdFALSE, msecs * portTICK_PERIOD_MS ) > 0;
}

const char* Cpl::System::Thread::myName() noexcept
{
    // Provide some protection in case this method is called before the scheduler is running (e.g. called by the trace engine)
    if ( !Cpl::System::Api::isSchedulingEnabled() )
    {
        return emptyString_;
    }

    return ( ( Cpl::System::FreeRTOS::Thread* ) xTaskGetApplicationTaskTag( xTaskGetCurrentTaskHandle() ) )->m_name;
}


size_t Cpl::System::Thread::myId() noexcept
{
    // Provide some protection in case this method is called before the scheduler is running (e.g. called by the trace engine)
    if ( !Cpl::System::Api::isSchedulingEnabled() )
    {
        return 0;
    }

    return (size_t) ( ( ( Cpl::System::FreeRTOS::Thread* )( &getCurrent() ) )->m_threadHandle );
}


void** Thread::getTlsArray() noexcept
{
    return ( ( Cpl::System::FreeRTOS::Thread* ) xTaskGetApplicationTaskTag( xTaskGetCurrentTaskHandle() ) )->m_tlsArray;
}


//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) noexcept
{
    Cpl::System::Mutex::ScopeBlock mylock( Cpl::System::Locks_::sysLists() );
    Cpl::System::FreeRTOS::Thread* t = threadList_.first();
    while ( t )
    {
        if ( client.item( *t ) == Cpl::Type::Traverser::eABORT )
        {
            break;
        }
        t = threadList_.next( *t );
    }
}


void addThreadToActiveList_( Thread& thread )
{
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::sysLists() );
    threadList_.put( thread );
}

void removeThreadFromActiveList_( Thread& thread )
{
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::sysLists() );
    threadList_.remove( thread );
}


//////////////////////////////
Cpl::System::Thread* Cpl::System::Thread::create( Runnable&   runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks
)
{
    return new (std::nothrow) Cpl::System::FreeRTOS::Thread( runnable, name, priority, stackSize );
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
