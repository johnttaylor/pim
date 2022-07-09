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
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Private_.h"

//
using namespace Cpl::System::BareMetal;

//
static Thread* mainThread_ = 0;

#define MAIN_THREAD_NAME    "MAIN"

////////////////////////////////////
namespace {

/// This class is used to turn the entry/native/main thread into a Cpl::System::Thread (i.e. add the thread semaphore)    
class RegisterInitHandler_ : public Cpl::System::StartupHook_,
    public Cpl::System::Runnable
{
protected:
    // Empty run function -- it is never called!
    void appRun() {}

public:
    ///
    RegisterInitHandler_():StartupHook_( eSYSTEM ) { m_running = true; }


protected:
    ///
    void notify( InitLevel_T init_level )
    {
        // Create a thread object for the native thread
        mainThread_ = new Thread( *this );
    }

};
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


////////////////////////////////////
Thread::Thread( Cpl::System::Runnable& dummyRunnable )
    :m_runnable( &dummyRunnable )
{
}

Thread::~Thread()
{
    // Nothing needed
}

Cpl::System::Runnable& Thread::setRunnable( Cpl::System::Runnable& newRunnableInstance )
{
    Runnable& prev          = *(mainThread_->m_runnable);
    mainThread_->m_runnable = &newRunnableInstance;
    return prev;
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
    return MAIN_THREAD_NAME;
}

size_t Thread::getId() noexcept
{
    return 0;
}

bool Thread::isRunning() noexcept
{
    return true;
}

Cpl::System::Runnable& Thread::getRunnable( void ) noexcept
{
    return *m_runnable;
}

Cpl_System_Thread_NativeHdl_T Thread::getNativeHandle( void ) noexcept
{
    return 0;
}


//////////////////////////////


//////////////////////////////
Cpl::System::Thread& Cpl::System::Thread::getCurrent() noexcept
{
    // Trap potential error
    if ( !mainThread_ )
    {
        Cpl::System::FatalError::logRaw( "BareMeal::Thread::getCurrent().  Cpl::System::Api::initialize() has NOT been called" );
    }

    return *mainThread_;
}


void Cpl::System::Thread::wait() noexcept
{
    mainThread_->m_syncSema.wait();
}

bool Cpl::System::Thread::tryWait() noexcept
{
    return mainThread_->m_syncSema.tryWait();
}

bool Cpl::System::Thread::timedWait( unsigned long timeout ) noexcept
{
    return mainThread_->m_syncSema.timedWait( timeout );
}

const char* Cpl::System::Thread::myName() noexcept
{
    return MAIN_THREAD_NAME;
}


size_t Cpl::System::Thread::myId() noexcept
{
    return 0;
}



//////////////////////////////
void Cpl::System::Thread::traverse( Cpl::System::Thread::Traverser& client ) noexcept
{
    client.item( *mainThread_ );
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
    return 0;
}


void Cpl::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
