#ifndef Cpl_System_GlobalLock_h_
#define Cpl_System_GlobalLock_h_
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


///
namespace Cpl {
///
namespace System {

/** This class defines an interface for a "Global Lock".  A Global Lock provides
    mutual exclusion and/or a critical section protection similar to a Mutex
    except that is it intended to be lighter weight and/or have faster
    performance than a traditional Mutex.  However, the performance gain comes
    with the  following constraints:

        o Non recursive semantics.  The calling thread CANNOT attempt to acquire
          the lock a second time once it has already acquire the lock.

        o The code that is protected by this lock MUST BE VERY SHORT time
          wise and NOT call an operating system methods (e.g. any Cpl::System
          methods).

    Why the above the constraints?  The GlobalLock interface is intended to
    be an abstraction for disable/enable interrupts when running on a RTOS
    platform. So, use GlobalLocks with care and always honor the above constraints.
 */
class GlobalLock
{

public:
    /** This method is invoked prior to entering a critical
        section.  If another thread currently "owns" the
        lock, the current thread will "wait" until it
        can obtain ownership before proceeding.
     */
    void static begin( void );

    /** This method is invoke at the end of a critical
        section.  This call will release the ownership of
        the lock.
     */
    void static end( void );

};



};      // end namespaces
};
#endif  // end header latch
