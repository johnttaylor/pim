#ifndef Cpl_System_Api_h_
#define Cpl_System_Api_h_
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


/** This class defines methods for initializing the Colony.Core class library
    and other startup/init actions.

    NOTE: The Colony.Core class library MUST BE INITIALIZED before calling
          any other Colony.Core methods/class/functions!
 */
class Api
{
public:
    /** This function initializes the Colony.Core class library.  This
        function should be called as soon possible on start-up/boot-up of the
        application.

        NOTES:

            o This method MUST be called BEFORE any other methods in the
              Colony.Core package is called! The only exception to this rule
              is for internal Colony.Core use only.
            o This method should only be called ONCE!
     */
    static void initialize( void );


    /** This function is used to initiate thread scheduling.  The semantics of
        this call is that thread scheduling is guarantied to occur AFTER this
        call.  However thread scheduling could have already begun/started PRIOR
        to this call. The actual behavior is platform specific.

        NOTE: This function MAY or MAY NOT return.  The specific behavior is
              platform specific.  It is the application's responsible to properly
              handle the function's 'return behavior'
     */
    static void enableScheduling( void );

    /** This function returns true if scheduling has enabled using the enableScheduling()
        functions.  When this function returns true, the application is guaranteed that
        current execution context is a CPL thread (assuming the application did not create
        any threads outside of the CPL Threading interface).
     */
    static bool isSchedulingEnabled( void );


public:
    /** This method causes the current thread to be suspended for
        n milliseconds.
     */
    static void sleep( unsigned long milliseconds ) noexcept;

    /** This method is the same as sleep(), EXCEPT that is guaranteed to
        suspend in 'real time'.  Typically an application NEVER needs
        to call this method.  See the SimTick interface for additional
        details about Simulated Time.
     */
    static void sleepInRealTime( unsigned long milliseconds ) noexcept;
};


};      // end namespaces
};
#endif  // end header latch

