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

#include "Restartable.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Mutex.h"
#include <new>


///
using namespace Cpl::TShell;


/// Anonymous namespace
namespace {

class Shell_ : public Cpl::System::Runnable
{
public:
    ///
    Cpl::System::Mutex          m_lock;
    ///
    Cpl::TShell::ProcessorApi&  m_shell;
    ///
    Cpl::Io::Input&				m_infd;
    ///
    Cpl::Io::Output&			m_outfd;
    ///
    Cpl::System::Thread*        m_myThread;
    ///
    bool                        m_startEnabled;
    ///
    bool                        m_endThread;
    ///
    bool                        m_shellRunning;

public:
    Shell_( Cpl::TShell::ProcessorApi& shell, Cpl::Io::Input& infd, Cpl::Io::Output& outfd, bool enabledOnLaunch )
        : m_shell( shell )
        , m_infd( infd )
        , m_outfd( outfd )
        , m_startEnabled( enabledOnLaunch )
        , m_endThread( false )
        , m_shellRunning( false )
    {
    }

public:
    ///
    void pleaseStop( bool stopThread = false )
    {
        m_lock.lock();
        m_endThread = stopThread;
        m_lock.unlock();
        m_shell.requestStop();
    }

    ///
    bool isRunning() noexcept
    {
        Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
        return m_shellRunning;
    }

    ///
    void appRun()
    {
        m_myThread = &Cpl::System::Thread::getCurrent();

        for ( ;;)
        {
            // Support starting in the disabled/wait state
            if ( m_startEnabled )
            {
                m_lock.lock();
                m_shellRunning = true;
                m_lock.unlock();
                
                m_shell.start( m_infd, m_outfd );
                
                m_lock.lock();
                m_shellRunning = false;
                m_lock.unlock();
            }
            m_startEnabled = true;
            if ( !m_endThread )
            {
                Cpl::System::Thread::wait();
            }
        }
    }

    /// 
    void restart()
    {
        if ( m_myThread )
        {
            m_myThread->signal();
        }
    };
};

}; // end anonymous namespace


/////////////////////////////////////////
Restartable::Restartable( Cpl::TShell::ProcessorApi& shell, const char* threadName, int threadPriority, bool thisIsAStaticInstance ) noexcept
    : Stdio( shell, threadName, threadPriority, thisIsAStaticInstance )
{
}

/////////////////////////////////////////
void Restartable::launch( Cpl::Io::Input& infd, Cpl::Io::Output& outfd, bool enabledOnLaunch ) noexcept
{
    // If I am re-launched -->kill the previous shell
    delete m_runnablePtr;
    if ( m_threadPtr )
    {
        Cpl::System::Thread::destroy( *m_threadPtr );
    }

    m_runnablePtr = new( std::nothrow ) Shell_( m_shell, infd, outfd, enabledOnLaunch );
    m_threadPtr   = Cpl::System::Thread::create( *m_runnablePtr, m_name, m_priority );
}

void Restartable::restart() noexcept
{
    if ( m_threadPtr && m_runnablePtr )
    {
        Shell_* myShellPtr = (Shell_*) m_runnablePtr;
        myShellPtr->restart();
    }
}
 
void Restartable::pleaseStop( bool stopThread ) noexcept
{
    if ( m_threadPtr && m_runnablePtr )
    {
        Shell_* myShellPtr = (Shell_*) m_runnablePtr;
        myShellPtr->pleaseStop( stopThread );
    }
}

bool Restartable::isRunning() noexcept
{
    if ( m_threadPtr && m_runnablePtr )
    {
        Shell_* myShellPtr = (Shell_*) m_runnablePtr;
        return myShellPtr->isRunning();
    }

    return false;
}
