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
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Private_.h"
#include "Cpl/System/Api.h"
#include "Cpl/Memory/Aligned.h"
#include "pico/platform.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include <new>
//
using namespace Cpl::System::RP2040;


// Internal states
#define THREAD_STATE_DOES_NOT_EXIST 0
#define THREAD_STATE_ALLOCATED      1
#define THREAD_STATE_CREATED        2
#define THREAD_STATE_RUNNING        3


typedef Cpl::Memory::AlignedClass<Thread> ThreadMem_T;

// Private variables
static volatile bool                schedulingEnabled_;
static volatile unsigned            states_[CPL_SYSTEM_RP2040_NUM_CORES];
static ThreadMem_T                  threadMemory_[CPL_SYSTEM_RP2040_NUM_CORES];
static Cpl::System::RP2040::Thread* threads_[CPL_SYSTEM_RP2040_NUM_CORES];

static void core1Entry( void );
inline static void launchCore1()
{
    multicore_launch_core1( core1Entry );
}

////////////////////////////////////
namespace {

// This class is used to allocate the actual thread instances.  In addition it
// has the side effect of turn the initial entry/native/main 'thread' into a 
// Cpl::System::Thread (i.e. adds the thread semaphore)    
class RegisterInitHandler_ : public Cpl::System::StartupHook_,
    public Cpl::System::Runnable
{
protected:
    // Empty run function -- it is never called!
    void appRun() {}

public:
    ///
    RegisterInitHandler_() :StartupHook_( eSYSTEM ) { m_running = true; }


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        // Create a thread objects
        Thread::createThreadInstance( 0, *this, "core0" );
        Thread::createThreadInstance( 1, *this, "core1" );
        states_[0] = THREAD_STATE_ALLOCATED;
        states_[1] = THREAD_STATE_ALLOCATED;
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;

void Thread::createThreadInstance( unsigned coreId, Cpl::System::Runnable& runnable, const char* name ) noexcept
{
    threads_[coreId] = new(threadMemory_[coreId].m_byteMem) Thread( runnable, name, coreId );
}


////////////////////////////////////
Thread::Thread( Cpl::System::Runnable& runnable, const char* name, unsigned coreId )
    : m_runnable( &runnable )
    , m_name( name )
    , m_coreId( coreId )
{
}

Thread::~Thread()
{
    // Nothing needed
}

//////////////////////////////
void Cpl::System::Api::enableScheduling( void )
{
    // Do nothing if called twice
    if ( !schedulingEnabled_ )
    {
        // Fail if the application has NOT 'created' core0 thread
        if ( states_[0] != THREAD_STATE_CREATED )
        {
            Cpl::System::FatalError::log( "The Application has NOT created any threads" );
        }

        // Housekeeping
        schedulingEnabled_ = true;

        // start core1 if it has been created
        if ( states_[1] == THREAD_STATE_CREATED )
        {
            launchCore1();
        }

        // start core0 
        multicore_lockout_victim_init();    // Enable SDK support on core0 for 'suspending scheduling'
        states_[0] = THREAD_STATE_RUNNING;
        threads_[0]->getRunnable().run();

        // If thread0/core0 runs to completion - force a cold boot
        watchdog_enable( 1, 1 );
        while ( 1 );
    }
}

void core1Entry( void )
{
    multicore_lockout_victim_init();    // Enable SDK support on core1 for 'suspending scheduling'
    states_[1] = THREAD_STATE_RUNNING;
    threads_[1]->getRunnable().run();   // Execute the Runnable object
    multicore_reset_core1();            // Self terminate if/when the Runnable object completes its processing
    states_[1] = THREAD_STATE_ALLOCATED;
}

bool Cpl::System::Api::isSchedulingEnabled( void )
{
    return schedulingEnabled_;
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
    return m_coreId;
}

bool Thread::isRunning() noexcept
{
    return m_runnable->isRunning();
}

Cpl::System::Runnable& Thread::getRunnable( void ) noexcept
{
    return *m_runnable;
}

Cpl_System_Thread_NativeHdl_T Thread::getNativeHandle( void ) noexcept
{
    return m_coreId;
}


//////////////////////////////


//////////////////////////////
Cpl::System::Thread& Cpl::System::Thread::getCurrent() noexcept
{
    // Note: All thread instances are created when the CPL library is initialized - so in theory there is always a valid current thread
    unsigned coreIdx = get_core_num();
    return *(threads_[coreIdx]);
}

void Cpl::System::Thread::wait() noexcept
{
    unsigned coreIdx = get_core_num();
    threads_[coreIdx]->m_syncSema.wait();
}

bool Cpl::System::Thread::tryWait() noexcept
{
    unsigned coreIdx = get_core_num();
    return threads_[coreIdx]->m_syncSema.tryWait();
}

bool Cpl::System::Thread::timedWait( unsigned long timeout ) noexcept
{
    unsigned coreIdx = get_core_num();
    return threads_[coreIdx]->m_syncSema.timedWait( timeout );
}

const char* Cpl::System::Thread::myName() noexcept
{
    unsigned coreIdx = get_core_num();
    return threads_[coreIdx]->m_name;
}

size_t Cpl::System::Thread::myId() noexcept
{
    unsigned coreIdx = get_core_num();
    return threads_[coreIdx]->m_coreId;
}


//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) noexcept
{
    Cpl::System::Mutex::ScopeBlock mylock( Cpl::System::Locks_::sysLists() );
    for ( unsigned idx=0; idx < CPL_SYSTEM_RP2040_NUM_CORES; idx++ )
    {
        if ( states_[idx] == THREAD_STATE_RUNNING )
        {
            if ( client.item( *(threads_[idx]) ) == Cpl::Type::Traverser::eABORT )
            {
                break;
            }
        }
    }
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
    // 'Create' the first thread
    if ( states_[0] == THREAD_STATE_ALLOCATED )
    {
        states_[0]              = THREAD_STATE_CREATED;
        threads_[0]->m_name     = name;
        threads_[0]->m_runnable = &runnable;
        return threads_[0];
    }

    // 'Create' the second thread
    else if ( states_[1] == THREAD_STATE_ALLOCATED )
    {
        states_[1]              = THREAD_STATE_CREATED;
        threads_[1]->m_name     = name;
        threads_[1]->m_runnable = &runnable;
        if ( schedulingEnabled_ )
        {
            launchCore1();
        }
        return threads_[1];
    }

    // If I get here then, ALL threads have already be 'created' -->so the fail the call
    return 0;
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    // Ignore request to destroy thread0/core0 thread
    if ( threadToDestroy.getNativeHandle() == 1 )
    {
        // Ignore if thread1/core1 is not running
        if ( states_[1] == THREAD_STATE_RUNNING )
        {
            // NOTE: In general it is not a good thing to "kill" threads - but to 
            //       let the thread "run-to-completion", i.e. have the run() method
            //       of the associated Runnable object complete.  If you do
            //       need to kill a thread - be dang sure that it is state such
            //       that it is ok to die - i.e. it has released all of its acquired
            //       resources: mutexes, semaphores, file handles, etc.
            multicore_reset_core1();
            states_[1] = THREAD_STATE_ALLOCATED;
        }
    }
}
