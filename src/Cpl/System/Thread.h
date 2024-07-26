#ifndef Cpl_System_Thread_h_
#define Cpl_System_Thread_h_
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

#include "colony_map.h"
#include "Cpl/System/Runnable.h"
#include "Cpl/System/Signable.h"
#include "Cpl/Type/Traverser.h"
#include <stdlib.h>


/// Defer the definition of the native thread handle to the application's 'platform'
#define Cpl_System_Thread_NativeHdl_T               Cpl_System_Thread_NativeHdl_T_MAP


/// The highest possible thread priority
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST          CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP

/// The recommended/default priority for a thread
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL           CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP

/// The lowest possible thread priority
#define CPL_SYSTEM_THREAD_PRIORITY_LOWEST           CPL_SYSTEM_THREAD_PRIORITY_LOWEST_MAP

/** This value can be 'added' to one of the above Priorities to increase (i.e.
    higher) the priority by one 'level. A priority can be increased multiple
    levels by adding this value multiple times.
 */
#define CPL_SYSTEM_THREAD_PRIORITY_RAISE            CPL_SYSTEM_THREAD_PRIORITY_RAISE_MAP

 /** This value can be 'added' to one of the above Priorities to decrease (i.e.
     lower) the priority by one 'level. A priority can be decreased multiple
     levels by adding this value multiple times.
  */
#define CPL_SYSTEM_THREAD_PRIORITY_LOWER            CPL_SYSTEM_THREAD_PRIORITY_LOWER_MAP



  ///
namespace Cpl {
///
namespace System {


/** This abstract class defines the operations that can be performed
    on a thread.  NOTE: The thread contains a built in counting semaphore
    that is usable by the developer.  This semaphore can be used to assist
    in implementing inter-thread-communications and/or ISR-to-thread
    communications.
 */
class Thread : public Signable
{
public:
    /// Virtual destructor.
    virtual ~Thread() {};

    /// This method returns the name (null terminated string) of the current thread.
    virtual const char* getName() noexcept = 0;

    /// This method returns the task's unique identifier
    virtual size_t getId() noexcept = 0;

    /** This method returns the 'running' state of the thread.  If the method
        returns false, the underlying thread has terminated (i.e. the run()
        method has completed) and then the Thread object/instance can be safely
        deleted using the destroy() method below.
     */
    virtual bool isRunning( void ) noexcept = 0;

    /** Returns the native thread handle.  Note: This handle can NOT be used
        a handle/reference/pointer to a Thread object instance.
     */
    virtual Cpl_System_Thread_NativeHdl_T getNativeHandle( void ) noexcept = 0;

    /// This method returns a reference to the thread's Runnable object
    virtual Runnable& getRunnable( void ) noexcept = 0;

public:
    /// This method returns a reference to the currently executing thread.
    static Thread& getCurrent() noexcept;

    /** Special version of getCurrent().  getCurrent() will trigger a FATAL
        error when called from a NON CPL thread.  tryGetCurrent() will 
        return a nullptr when it detects that the current thread is NOT a 
        CPL thread.

        This method should ONLY be used when your application has mix of CPL and
        non-CPL threads, i.e. the default should be to use getCurrent(). 
     */
    static Thread* tryGetCurrent() noexcept;

    /** This method causes the CURRENT thread to wait until its 'thread
        semaphore' is signal/set.
     */
    static void wait() noexcept;

    /** This method is similar to Semaphore::tryWait(), except that it operates
        on the thread semaphore
     */
    static bool tryWait() noexcept;

    /** Same as wait(), except the call will return after 'timeoutInMsec'
        has expired without the thread being signaled.  The method return true
        if the Thread was signaled; else false is returned if the timeout period
        expired.
     */
    static bool timedWait( unsigned long timeoutInMsec ) noexcept;

    /** Returns the name for the current thread (i.e. short-hand for
        threadPtr->getCurrent().getName()
     */
    static const char* myName() noexcept;

    /** Returns the ID for the current thread (i.e. short-hand for
        threadPtr->getCurrent().getId()
     */
    static size_t myId() noexcept;

    /// This method returns a reference to the current thread' runnable instance.
    static inline Runnable& myRunnable() noexcept { return getCurrent().getRunnable(); }


public:
    /** This abstract class defines the client interface for walking
        the list of threads, i.e. defines the callback method for
        when walking/traversing the list of active threads.
     */
    class Traverser
    {
    public:
        /// Virtual Destructor
        virtual ~Traverser() {}

    public:
        /** This method is called once for ever item in the "traversee's"
            list.  The return code from the method is used by the traverser
            to continue the traversal (eCONTINUE), or abort the traversal
            (eABORT).
         */
        virtual Cpl::Type::Traverser::Status_T item( Thread& nextThread ) =0;
    };


    /** Internal Iterator that allows the Client to traverse the list
        active threads.

        NOTES:
            o There is difference between an "active" thread and a
              Thread object instance.  The underlying platform/OS
              determines if the a thread is "active", i.e. the thread
              is created and running.  The Thread object gets created
              BEFORE the thread is active and exists AFTER thread
              terminates/end.  Why the explanation, this method only
              includes "active" threads.  Threads that have terminated,
              but that their associated Thread object instance has not
              been delete/destroyed will NOT show up in the active list.
            o This call is mutex protected.  It will prevent new
              threads from being created/deleted until it has
              completed.
     */
    static void traverse( Thread::Traverser& client ) noexcept;


public:

    /** This method creates a thread according to the specified parameters.  The
        thread/task begins execution immediately upon its creation.

        NOTE: Not all platform support all options, e.g. The C++11 multi
              threading support does not directly support thread priorities.


        @returns A pointer to the Thread created, OR 0 if the was an error.

        @param runnable       a runnable object which is started when the task is created
        @param name           a null terminated string that represent that thread's name
        @param priority       a priority ranging from highest priority (CPL_SYSTEM_THREAD_PRIORITY_HIGHEST)
                              to lowest priority (CPL_SYSTEM_THREAD_PRIORITY_LOWEST).
                              Priorities are defined in relative 'levels' to the max/normal/min
                              settings because there is no standardized priority settings
                              across Operating Systems.  If the priority value exceeds the
                              upper/lower bounds the platform's priority range, the thread's
                              priority will be clamped to the upper/lower bound of the priority
                              range.
        @param stackSize      Requested size, in bytes, of the stack for the thread.  A value of
                              of zero will size the thread's stack per the platform's default
                              stack size setting.
        @param stackPtr       Pointer to the stack memory to be used for the thread's stack. If
                              zero is passed, then the underlying platform/OS will allocate the
                              stack memory.
        @param allowSimTicks  With this argument is true the thread will be created to
                              use/support simulated time when 'Simulate ticks' are enabled.
                              A value false will force the thread to always be a 'real time'
                              thread.  See Cpl::System::SimTicks for more details about
                              simulated time.
     */
    static Thread* create( Runnable&   runnable,
                           const char* name,
                           int         priority      = CPL_SYSTEM_THREAD_PRIORITY_NORMAL,
                           int         stackSize     = 0,
                           void*       stackPtr      = 0,
                           bool        allowSimTicks = true
    );


    /** This method is used to destroy a thread that was created by the Thread
        Factory. If you are going to delete a thread that was created by the
        Thread Factory, you MUST use the Factory's destroy method to delete it
        to properly insure that the memory used to create the task is released
        correctly.

        NOTES:

            o The application should only delete/destroy a thread AFTER the
              thread's Runnable object.run() method has ended/terminated.  If
              the thread is destroy/delete before run() has completed, there is
              NO GUARANTEE with respect to whether or not the thread has
              released all acquired resources!
            o NOT ALL Platforms support brute-force termination of threads,
              e.g. in C++11 there is no method/interface to forcibly terminate
              a thread.  Once again -->it is a HIGHLY DISCOURAGED to design
              your application where you need to forcibly terminate threads.
            o This method only deletes/destroys the Thread instance -- it does
              NOT delete/destroy the associated Runnable instance.
     */
    static void destroy( Thread& threadToDestroy );

};


};      // end namespaces
};
#endif  // end header latch
