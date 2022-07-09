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

#include "Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Container/SList.h"
#include <process.h>

//
using namespace Cpl::System::Win32;

// Initialize static class variables
DWORD               		            dwTlsIndex_ = 0xFFFFFFFF;
bool                		            keyCreated_ = false;
static Cpl::Container::SList<Thread>  	threadList_( "StaticConstructor" );

static void addThreadToActiveList_( Thread& thread );
static void removeThreadFromActiveList_( Thread& thread );


////////////////////////////////////
namespace {

/// This class is used to turn the entry/native/main thread into a Cpl::System::Thread (i.e. add the thread semaphore)    
class RegisterInitHandler_ : public Cpl::System::StartupHook_,
    public Cpl::System::Runnable
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
    ///
    RegisterInitHandler_():StartupHook_( eSYSTEM ) {}


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        // Create a thread object for the native thread
        m_running = true;
        new Thread( *this );
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


////////////////////////////////////
Thread::Thread( Cpl::System::Runnable& dummyRunnable )
    :m_runnable( dummyRunnable ),
    m_name( "Win32Main" ),
    m_threadID( GetCurrentThreadId() ),
    m_threadHandle( 0 ),
    m_priority( GetThreadPriority( (HANDLE) m_threadID ) )
{
    // Create TSD key that holds a pointer to "me" (only do this once)
    // NOTE: By definition/design this constructor will be CALLED before
    //       the 'normal' thread constructor and will ONLY CALLED ONCE.
    dwTlsIndex_ = TlsAlloc();
    if ( dwTlsIndex_ == TLS_OUT_OF_INDEXES )
    {
        Cpl::System::FatalError::logRaw( "Win32::Thread::Thread().  TlsAlloc() failed when creating index to store the current Thread Pointer instance." );
    }

    // Set my Tls index/key 'created' status to good
    keyCreated_ = true;

    // Store a reference to the Cpl thread object for the native thread
    TlsSetValue( dwTlsIndex_, this );

    // Add the native thread to the list of active threads
    addThreadToActiveList_( *this );

    // Mark the NATIVE/Main thread as 'real time' thread for the SimTick engine
    CPL_SYSTEM_SIM_TICK_THREAD_INIT_( false );

    // Get a 'Real Handle' to this thread
    DuplicateHandle( GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &m_threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS );
}


Thread::Thread( Cpl::System::Runnable&   runnable,
                const char*              name,
                int                      priority,
                unsigned                 stackSize,
                bool                     allowSimTicks
)
    :m_runnable( runnable ),
    m_name( name ),
    m_threadID( 0 ),
    m_threadHandle( 0 ),
    m_priority( priority ),
    m_allowSimTicks( allowSimTicks )
{
    // Create a thread
    m_threadHandle = (HANDLE) _beginthread( entryPoint, stackSize, this );
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
            TerminateThread( m_threadHandle, 0 );
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
//DWORD WINAPI Thread::entryPoint(void* data)
void __cdecl Thread::entryPoint( void* data )
{
    // Initialize the TLS Value for this thread. 
    TlsSetValue( dwTlsIndex_, data );

    // Go Execute the "Runnable" object
    Thread* myThreadPtr     = (Thread*) data;
    myThreadPtr->m_threadID = GetCurrentThreadId();
    SetThreadPriority( GetCurrentThread(), myThreadPtr->m_priority );
    addThreadToActiveList_( *myThreadPtr );
    myThreadPtr->m_runnable.setThreadOfExecution_( myThreadPtr );
    CPL_SYSTEM_SIM_TICK_THREAD_INIT_( myThreadPtr->m_allowSimTicks );
    myThreadPtr->m_runnable.run();
    CPL_SYSTEM_SIM_TICK_ON_THREAD_EXIT_();

    // Remove the thread from the list of active threads
    removeThreadFromActiveList_( *myThreadPtr );
}


//////////////////////////////
Cpl::System::Thread& Cpl::System::Thread::getCurrent() noexcept
{
    // Trap potential error
    if ( !keyCreated_ )
    {
        Cpl::System::FatalError::logRaw( "Win32::Thread::getCurrent().  Have not yet created 'Tls Index'." );
    }

    Thread* curThread = (Thread*) TlsGetValue( dwTlsIndex_ );

    // Trap potential error
    if ( !curThread )
    {
        Cpl::System::FatalError::logRaw( "Win32::Thread::getCurrent().  Current thread is NOT a 'Cpl::System::Thread'." );
    }

    return *curThread;
}


void Cpl::System::Thread::wait() noexcept
{
    ((Cpl::System::Win32::Thread*)(&getCurrent()))->m_syncSema.wait();
}

bool Cpl::System::Thread::tryWait() noexcept
{
    return ((Cpl::System::Win32::Thread*)(&getCurrent()))->m_syncSema.tryWait();
}

bool Cpl::System::Thread::timedWait( unsigned long timeout ) noexcept
{
    return ((Cpl::System::Win32::Thread*)(&getCurrent()))->m_syncSema.timedWait( timeout );
}

const char* Cpl::System::Thread::myName() noexcept
{
    return ((Cpl::System::Win32::Thread*)(&getCurrent()))->m_name;
}

size_t Cpl::System::Thread::myId() noexcept
{
    return ((Cpl::System::Win32::Thread*)(&getCurrent()))->m_threadID;
}


//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) noexcept
{
    Cpl::System::Mutex::ScopeBlock mylock( Cpl::System::Locks_::sysLists() );
    Cpl::System::Win32::Thread* t = threadList_.first();
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
    return new Cpl::System::Win32::Thread( runnable, name, priority, stackSize, allowSimTicks );
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
