#ifndef Cpl_System_Semaphore_h_
#define Cpl_System_Semaphore_h_
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
#include "Cpl/System/Signable.h"


/// Defer the definition of the a raw semaphore type to the application's 'platform'
#define Cpl_System_Sema_T          Cpl_System_Sema_T_MAP


///
namespace Cpl {
///
namespace System {

/** This semaphore class defines the interface for a Counting Semaphore.

    NOTES:

        o The 'max-count' for a Semaphore is platform specific.
        o The class inherits from 'Signable' which contains/defines the
          "signal()" methods.
 */
class Semaphore : public Signable
{
public:
    /** Constructor.  The semaphore is created with the specified
        'initialCount'.  A count of zero, will cause an immediate
        call to 'wait()' to block.
     */
    Semaphore( unsigned initialCount=0 );

    /// Destructor
    ~Semaphore();


public:
    /** This method causes the current thread to block and wait till the
        semaphore is signaled/set.  NOTE: Can only be called from a Thread
        context!
     */
    void wait( void ) noexcept;

    /** This method is the same as wait(), except that 'timeout' specifies the
        maximum amount of time, in milliseconds, will block if the semaphore
        count is zero.  The method return true if the semaphore was signaled
        (i.e. count > 0); else false is returned if the timeout period
        expired.
     */
    bool timedWait( unsigned long timeout ) noexcept;

    /** This method is the same as wait(), except that if the semaphore
        count is zero, the method returns immediately and has a return code
        of false.  If the semaphore count is greater than zero, the count
        is decrement and the method returns immediately and has a return code
        of true.
     */
    bool tryWait( void ) noexcept;


public:
    /// Signable API    
    int signal( void ) noexcept;

    /// Signable API    
    int su_signal( void ) noexcept;


protected:
    /// Raw Semaphore handle/instance/pointer
    Cpl_System_Sema_T  m_sema;


protected:
    /// Helper method for supporting SimTicks and 'real' tick in the same build
    void waitInRealTime( void ) noexcept;

    /// Helper method for supporting SimTicks and 'real' tick in the same build
    bool timedWaitInRealTime( unsigned long timeout ) noexcept;

private:
    /// Prevent access to the copy constructor -->semaphores can not be copied!
    Semaphore( const Semaphore& m );

    /// Prevent access to the assignment operator -->semaphores can not be copied!
    const Semaphore& operator=( const Semaphore& m );


    /// Friends(s)
    friend class Api;
    friend class SimTick;
};


};      // end namespaces
};
#endif  // end header latch
