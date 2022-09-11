#ifndef Cpl_System_Runnable_h_
#define Cpl_System_Runnable_h_
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

///
namespace Cpl {
///
namespace System {


/// Forward class reference to avoid a circular dependency.
class Thread;


/** This is an abstract class defines the interface for an object
    that is "executed" when a Thread object is created.
 */
class Runnable
{
public:
    /// Virtual destructor
    virtual ~Runnable() {};

    /** This method is called when the Thread is started.  If
        this function returns, the Thread will be terminated.
        This method is implemented by the base class, the application
        and/or child class needs to implement the 'appRun()'
        method.
     */
    virtual void run();

    /** This method is a request to have the runnable object
        terminate itself.  This is only a request, it is
        not a requirement of a runnable object to oblige the
        request.  Some runnable objects are just not 'built'
        to handle a such a request.  On the other hand, a runnable
        object such as a MailBoxServer is, and should make
        every attempt to comply with the request.

        NOTES:

            1) This a low-level terminate request in that it
               does not understand the "semantics" of what the
               thread is currently doing.  It simply attempts to
               exit the run() method.  The system must first shutdown
               the "application" of the thread BEFORE calling this
               method.
            2) This method is an attempted work-around for terminating
               threads by having the thread run itself to completion.
               Not all OSes support a polite way (i.e. reclaiming
               resource, memory, etc.) of killing threads.
     */
    virtual void pleaseStop() {}

    /** This method returns true if the instance has 'entered' its
        run method; false is returned, i.e. the run() method has
        completed.
     */
    virtual bool isRunning() noexcept;


protected:
    /// Constructor
    Runnable();

    /** This method is called from the run() method.  It is intended to
        to be the entry point for application specific code that is
        executed for the associated thread.
     */
    virtual void appRun() = 0;


protected:
    /// Tracks the run state of the instance
    bool m_running;

public:
    /** This method has COMPONENT Scope, it is only made public to avoid the
        tight coupling of the 'friend mechanism' for friending current and future
        concrete thread classes.  The application SHOULD NEVER call/use this
        method.

        This method is used to provide the runnable instance knowledge about
        what thread it is associated with. The primary motivation for this is
        to help terminate a thread when pleaseStop() is called. A default
        implementation is provided that does NOTHING.  It is up the concrete
        child class to decide how (if at all) to use this information.  The
        Cpl::System::Thread will call this method BEFORE the run() is called.
     */
    virtual void setThreadOfExecution_( Thread* myThreadPtr ) {}


};

};      // end namespaces
};
#endif  // end header latch
