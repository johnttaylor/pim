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
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Container/SList.h"
#include <limits.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>

//
using namespace Cpl::System::Posix;

// Initialize static class variables
pthread_key_t         		         tsdKey_;
bool                		         keyCreated_ = false;
static Cpl::Container::SList<Thread> threadList_( "StaticConstructor" );

static void addThreadToActiveList_( Thread& thread );
static void removeThreadFromActiveList_( Thread& thread );

// 'ole basic min/max methods
inline int myMin( int a, int b ) { return a < b ? a : b; }
inline int myMax( int a, int b ) { return a > b ? a : b; }

#define AREA_	"Cpl::System::Posix::Thread"

////////////////////////////////////
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
    RegisterInitHandler_():StartupHook_( eSYSTEM ) { m_running = true; }


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        // Create a thread object for the native thread
        new Thread( *this );
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


////////////////////////////////////
Thread::Thread( Cpl::System::Runnable& dummyRunnable )
    :m_runnable( dummyRunnable ),
    m_name( "PosixMain" ),
    m_threadID( (size_t) (pthread_self()) ),
    m_threadHandle( pthread_self() )
{
    // Create TSD key that holds a pointer to "me" (only do this once)
    // NOTE: By definition/design this constructor will be CALLED before
    //       the 'normal' thread constructor and will ONLY CALLED ONCE.
    if ( pthread_key_create( &tsdKey_, NULL ) != 0 )
    {
        Cpl::System::FatalError::logRaw( "Posix::Thread::Thread().  pthread_key_create() failed when creating index to store the current Thread Pointer instance." );
    }

    // Set my Tls index/key 'created' status to good
    keyCreated_ = true;

    // Store a reference to the Cpl thread object for the native thread
    pthread_setspecific( tsdKey_, this );

    // Add the native thread to the list of active threads
    addThreadToActiveList_( *this );

    // Mark the NATIVE/Main thread as 'real time' thread for the SimTick engine
    CPL_SYSTEM_SIM_TICK_THREAD_INIT_( false );
}


Thread::Thread( Cpl::System::Runnable&   runnable,
                const char*              name,
                int                      priority,
                unsigned                 stackSize,
                int                      schedType,
                bool                     allowSimTicks
)
    :m_runnable( runnable ),
    m_name( name ),
    m_threadID( 0 ),
    m_allowSimTicks( allowSimTicks ),
    m_threadHandle( 0 )
{
    CPL_SYSTEM_TRACE_FUNC( AREA_ );
    CPL_SYSTEM_TRACE_MSG( AREA_, ("Name=%s, pri=%d, schedType=%d", name, priority, schedType) );

    // Calculate run time priority
    struct sched_param threadPriority;
    int platMinPriority = sched_get_priority_min( schedType );
    int platMaxPriority = sched_get_priority_max( schedType );
    priority += platMinPriority;
    priority  = myMin( priority, platMaxPriority );
    priority  = myMax( priority, platMinPriority );
    threadPriority.sched_priority = priority;
    CPL_SYSTEM_TRACE_MSG( AREA_, ("platMinPri=%d platMaxPri=%d, runTimePri=%d", platMinPriority, platMaxPriority, priority) );

    // Setup to access the Thread Attributes
    pthread_attr_t thread_attr;
    int rc_init = pthread_attr_init( &thread_attr );

    // Get the default Stack size
    size_t defaultStackSize = 0;
    pthread_attr_getstacksize( &thread_attr, &defaultStackSize );
    if ( stackSize == 0 )
    {
        stackSize = defaultStackSize;
    }

    // Initialize Thread Attributes
    int rc_setdetach   = pthread_attr_setdetachstate( &thread_attr, PTHREAD_CREATE_DETACHED );   // This is to prevent memory leaks
    int rc_setstack    = pthread_attr_setstacksize( &thread_attr, myMax( PTHREAD_STACK_MIN, stackSize ) );
    int rc_schedpolicy = pthread_attr_setschedpolicy( &thread_attr, schedType );
    int rc_schedparam  = pthread_attr_setschedparam( &thread_attr, &threadPriority );
    CPL_SYSTEM_TRACE_MSG( AREA_, ("pthread_attr_* Results (0==good): _init()=%d, _setdetachedstate=%d, _setstacksize=%d, _setschedpolicy=%d, _setschedparam=%d, stacksize=%d", rc_init, rc_setdetach, rc_setstack, rc_schedpolicy, rc_schedparam, myMax( PTHREAD_STACK_MIN, stackSize )) );
    if ( rc_init || rc_setdetach || rc_setstack || rc_schedpolicy || rc_schedparam )
    {
        CPL_SYSTEM_TRACE_MSG( AREA_, ("Cpl::System::Posix::Thread. unexpected error when creating thread %s. Returns codes (all should be zero): attr_init=%d, setdetach=%d, setstack=%d, schedpolicy=%, schedparam=%d", name, rc_init, rc_setdetach, rc_setstack, rc_schedpolicy, rc_schedparam ));
    }


    // Create the thread
    pthread_create( &m_threadHandle, &thread_attr, &entryPoint, this );
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
            pthread_cancel( (pthread_t) m_threadID );
        }
    }
}


//////////////////////////////
int Thread::signal() noexcept
{
    return m_syncSema.signal();
}

int Thread::su_signal() noexcept
{
    return m_syncSema.su_signal();
}

const char* Thread::getName() noexcept
{
    return m_name;
}

size_t Thread::getId() noexcept
{
    return m_threadID;
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
void* Thread::entryPoint( void* data )
{
    // Convert data arg to a pointer to a Thread Object
    Thread* myThreadPtr = (Thread*) data;

    // Set the my TSD key for this thread to point to "my thread"
    pthread_setspecific( tsdKey_, myThreadPtr );

    // Set my "cancel" type so that thread can always be killed (i.e. the thread
    // is terminated immediately without waiting to reach a "cancellation point")
    int oldCancelType;
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &oldCancelType );

    // Go Execute the "Runnable" object
    myThreadPtr->m_threadID = (size_t) (pthread_self());
    addThreadToActiveList_( *myThreadPtr );
    myThreadPtr->m_runnable.setThreadOfExecution_( myThreadPtr );
    CPL_SYSTEM_SIM_TICK_THREAD_INIT_( myThreadPtr->m_allowSimTicks );
    myThreadPtr->m_runnable.run();
    CPL_SYSTEM_SIM_TICK_ON_THREAD_EXIT_();

    // Remove the thread from the list of active threads
    removeThreadFromActiveList_( *myThreadPtr );
    return 0;
}


//////////////////////////////
Cpl::System::Thread& Cpl::System::Thread::getCurrent() noexcept
{
    // Trap potential error
    if ( !keyCreated_ )
    {
        Cpl::System::FatalError::logRaw( "Posix::Thread::getCurrent().  Have not yet created 'Tls Index'." );
    }

    Thread* curThread = (Thread*) pthread_getspecific( tsdKey_ );

    // Trap potential error
    if ( !curThread )
    {
        Cpl::System::FatalError::logRaw( "Posix::Thread::getCurrent().  Current thread is NOT a 'Cpl::System::Thread'." );
    }

    return *curThread;
}


void Cpl::System::Thread::wait() noexcept
{
    ((Cpl::System::Posix::Thread*)(&getCurrent()))->m_syncSema.wait();
}

bool Cpl::System::Thread::tryWait() noexcept
{
    return ((Cpl::System::Posix::Thread*)(&getCurrent()))->m_syncSema.tryWait();
}

bool Cpl::System::Thread::timedWait( unsigned long msecs ) noexcept
{
    return ((Cpl::System::Posix::Thread*)(&getCurrent()))->m_syncSema.timedWait( msecs );
}

const char* Cpl::System::Thread::myName() noexcept
{
    return ((Cpl::System::Posix::Thread*)(&getCurrent()))->m_name;
}

size_t Cpl::System::Thread::myId() noexcept
{
    return ((Cpl::System::Posix::Thread*)(&getCurrent()))->m_threadID;
}


//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) noexcept
{
    Cpl::System::Mutex::ScopeBlock mylock( Cpl::System::Locks_::sysLists() );
    Cpl::System::Posix::Thread* t = threadList_.first();
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
    return new Cpl::System::Posix::Thread( runnable, name, priority, stackSize, allowSimTicks );
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
