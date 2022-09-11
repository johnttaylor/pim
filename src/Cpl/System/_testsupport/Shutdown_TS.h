#ifndef Cpl_System_x_testsupport_Shutdown_TS_h_
#define Cpl_System_x_testsupport_Shutdown_TS_h_
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

#include <stddef.h>


/// 
namespace Cpl {
/// 
namespace System {


/** This class provides methods to access the unit testing support
    for the Cpl::System::Shutdown interface.

    This class provides an implementation of Shutdown interface that
    has the ability to count the number times the Shutdown interface
    is/was called instead of exiting.  It can also alter the application's
    exit code.

    Notes:
        o The Shutdown implementation assumes/uses the C Library 'exit()'
          method.
 */
class Shutdown_TS
{
public:
    /** This method zero's the exit counter and sets the shutdown behavior
        to NOT exit when called - only count the calls.  Note: This
        interface starts in the 'counter mode' with the count set to
        zero.
     */
    static void clearAndUseCounter( void );

    /** This method returns the current call count AND clears the
        call counter.
     */
    static size_t getAndClearCounter( void );


public:
    /** This method allows the application to exit - but always with
        the specified 'new_exit_code' (i.e. ignoring the application
        supplied exit code).  This method is helpful during unit testing
        for 'passing' and exit/failure use case.
     */
    static void setExitCode( int new_exit_code );

    /** This method restores the exit/shutdown behavior to its 'non-test'
        paradigm.
     */
    static void restore( void );

};


};      // end namespaces
};
#endif  // end header latch

