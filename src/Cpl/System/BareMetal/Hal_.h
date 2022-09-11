#ifndef Cpl_System_Baremtal_Hal_x_h__
#define Cpl_System_Baremtal_Hal_x_h__
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
/** @file

    This file defines a hardware abstraction layer (HAL) for the BareMetal
    implementation of Cpl::System component
*/


/// 
namespace Cpl {
/// 
namespace System {
///
namespace BareMetal {


/** This method is used to initialize the HAL/Target layer.  What needs to be
    an/or is initialized is platform specific.
 */
void initialize( void ) noexcept;

/** This method performs a busy-wait for the specified number of milliseconds
 */
void busyWait( unsigned long waitTimeMs ) noexcept;

/** This method returns the current elapsed time since power-up in milliseconds
 */
unsigned long getElapsedTime( void ) noexcept;

};      // end namespaces
};
};
#endif  // end header latch

