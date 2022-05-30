#ifndef Cpl_System_Mutex_h_
#define Cpl_System_Mutex_h_
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

#include "colony_map.h"

/// Defer the definition of the a raw mutex type to the application's 'platform'
#define Cpl_System_Mutex_T          Cpl_System_Mutex_T_MAP


///
namespace Cpl {
///
namespace System {

/** This mutex class defines the interface for a mutex that has "recursive"
    semantics.  Recursive semantic allows the thread that owns the mutex
    to acquire ownership multiple times by calling lock() multiple times.  There
    must be a corresponding number of unlock() calls made before the ownership
    of the mutex is released.
 */
class Mutex
{
public:
    /// Constructor
    Mutex();

    /// Destructor
    ~Mutex();

public:
    /** This method is invoked prior to entering a critical
        section.  If another thread currently "owns" the
        mutex, the current thread will "wait" until it
        can obtain ownership before proceeding.
     */
    void lock( void );

    /** This method is invoke at the end of a critical
        section.  This call will release the ownership of
        the mutex.
     */
    void unlock( void );


protected:
    /// Raw Mutex handle/instance/pointer
    Cpl_System_Mutex_T  m_mutex;


private:
    /// Prevent access to the copy constructor -->mutexes can not be copied!
    Mutex( const Mutex& m );

    /// Prevent access to the assignment operator -->mutexes can not be copied!
    const Mutex& operator=( const Mutex& m );


public:
    /** This concrete class provides a simple mechanism for providing mutex
        protection for a "scope block".  The class is instantiated as
        a local variable for the scope block it is meant to protected.  When
        this object is created - it calls lock() on its mutex.  When this
        object is destroyed - it calls unlock() on its mutex.
     */
    class ScopeBlock
    {
    private:
        /// Reference to the mutex to be used for synchronization
        Mutex & m_mutex;

    public:
        /// Constructor.  This will block until the mutex lock is acquired.
        inline ScopeBlock( Mutex& mutex ) noexcept:m_mutex( mutex ) { m_mutex.lock(); }

        /// Destructor.  This method releases ownership of the mutex
        inline ~ScopeBlock() { m_mutex.unlock(); }


    private:
        /** This method PREVENTS this object from being created on the Heap.
            This class can only/should only be instantiated as a local variable to
            a critical section scope block!
         */
        void* operator  new(size_t);
    };

};



};      // end namespaces
};
#endif  // end header latch
