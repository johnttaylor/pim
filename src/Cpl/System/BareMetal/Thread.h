#ifndef Cpl_System_BareMetal_Thread_h_
#define Cpl_System_BareMetal_Thread_h_
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
/** @file */

#include "Cpl/System/Thread.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/Text/FString.h"



///
namespace Cpl {
///
namespace System {
///
namespace BareMetal {

/** This concrete class implements 'enough' of a Thread object to support 
    Cpl::System framework on a bare metal system (a system that has only
    ONE thread and potentially ISR contexts).
 */
class Thread : public Cpl::System::Thread
{
protected:
    /// Reference to the runnable object for the thread
    Cpl::System::Runnable*  m_runnable;

    /// The thread synchronized message semaphore.
    Cpl::System::Semaphore  m_syncSema;


public:
    /** This method allows the application to set the Runnable object for
        for single-thread/execution context.  The method returns a reference
        to the previous/runnable object being 'replaced'
     */
    static Runnable& setRunnable( Runnable& newRunnableInstance );

public:
    /// Private Constructor -->the application can not create threads!
    Thread( Runnable& runnable, const char* name );

    /// Private Constructor -->the application can not create threads!
    Thread();

    /// Private Constructor -->the application can not create threads!
    Thread( Thread& copy );

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
    /** COMPONENT Scoped constructor to "convert" the current execution context
        to a Cpl Thread. THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
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
