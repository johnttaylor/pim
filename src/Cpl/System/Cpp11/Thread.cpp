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
#include "Cpl/System/FatalError.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Container/SList.h"


//
using namespace Cpl::System::Cpp11;

// Initialize static class variables
thread_local Thread*                    cpl_system_cpp11_thread_mePtr_ = 0;
static size_t                           threadIDSource_ = 1; // Initially start at 1, i.e. '0' is invalid thread ID
static Cpl::Container::SList<Thread>  	threadList_( "StaticConstructor" );

static void addThreadToActiveList_( Thread& thread );
static void removeThreadFromActiveList_( Thread& thread );

static size_t generatedThreadID_( void )
{
    Cpl::System::Mutex::ScopeBlock mylock( Cpl::System::Locks_::system() );
    size_t newID = threadIDSource_++;
    return newID;
}


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
    m_name( "Cpp11Main" ),
    m_threadID( generatedThreadID_() ),
    m_thread()
{
    // Store a reference to the Cpl thread object for the native thread
    cpl_system_cpp11_thread_mePtr_ = this;

    // Add the native thread to the list of active threads
    addThreadToActiveList_( *this );

    // Mark the NATIVE/Main thread as 'real time' thread for the SimTick engine
    CPL_SYSTEM_SIM_TICK_THREAD_INIT_( false );
}


Thread::Thread( Cpl::System::Runnable&   runnable,
                const char*              name,
                bool                     allowSimTicks

)
    :m_runnable( runnable ),
    m_name( name ),
    m_threadID( 0 ),
    m_allowSimTicks( allowSimTicks ),
    m_thread( Thread::entryPoint, this )
{
}

Thread::~Thread()
{
    // NOTE: In general it is not a good thing to "kill" threads - but to 
    //       let the thread "run-to-completion", i.e. have the run() method
    //       of the associated Runnable object complete.  If you do
    //       need to kill a thread - be dang sure that it is state such
    //       that it is OK to die - i.e. it has released all of its acquired
    //       resources: mutexes, semaphores, file handles, etc.
    if ( m_runnable.isRunning() )
    {
        // Ask the runnable object nicely to stop 
        m_runnable.pleaseStop();
        Cpl::System::Api::sleep( 100 );     // Yield execution and allow time for the thread to actually exit.
    }

    // NOTE: There is no (portably) mechanism in C++11 to forcibly terminate
    //       a thread.  'detach()' is called here so that std::terminate() does
    //       NOT get called if the thread is still active when the Thread object
    //       is deleted (terminate() will abort the entire application).
    m_thread.detach();
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
    return m_thread.native_handle();
}

Cpl::System::Runnable& Thread::getRunnable( void ) noexcept
{
    return m_runnable;
}

//////////////////////////////
void Thread::entryPoint( Thread* myThreadPtr )
{
    // Initialize the TLS Value for this thread. 
    cpl_system_cpp11_thread_mePtr_ = myThreadPtr;

    // Go Execute the "Runnable" object
    myThreadPtr->m_threadID = generatedThreadID_();
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
    if ( !cpl_system_cpp11_thread_mePtr_ )
    {
        Cpl::System::FatalError::logRaw( "Cpp11::Thread::getCurrent().  Current thread is NOT a 'Cpl::System::Thread'." );
    }

    return *cpl_system_cpp11_thread_mePtr_;
}


void Cpl::System::Thread::wait() noexcept
{
    ((Cpl::System::Cpp11::Thread*)(&getCurrent()))->m_syncSema.wait();
}

bool Cpl::System::Thread::tryWait() noexcept
{
    return ((Cpl::System::Cpp11::Thread*)(&getCurrent()))->m_syncSema.tryWait();
}

bool Cpl::System::Thread::timedWait( unsigned long timeout ) noexcept
{
    return ((Cpl::System::Cpp11::Thread*)(&getCurrent()))->m_syncSema.timedWait( timeout );
}

const char* Cpl::System::Thread::myName() noexcept
{
    return ((Cpl::System::Cpp11::Thread*)(&getCurrent()))->m_name;
}


size_t Cpl::System::Thread::myId() noexcept
{
    return ((Cpl::System::Cpp11::Thread*)(&getCurrent()))->m_threadID;
}


//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) noexcept
{
    Cpl::System::Mutex::ScopeBlock mylock( Cpl::System::Locks_::sysLists() );
    Cpl::System::Cpp11::Thread* t = threadList_.first();
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
    return new Cpl::System::Cpp11::Thread( runnable, name, allowSimTicks );
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
