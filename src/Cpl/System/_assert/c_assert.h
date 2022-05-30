#ifndef Cpl_System_Assert_C_Assert_
#define Cpl_System_Assert_C_Assert_
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

#include <stdbool.h>

#ifdef _cpluscplus
extern "C" {
#endif


/** These method is 'C wrapper' that calls Cpl::System::FatalError handler when
    the assert fails.  The C Wrapper is need to as not to pollute the Platform
    mapping header files with C++ headers that cause compile issues when the
    underlying Platform is C based RTOS (e.g. FreeRTOS)
 
 */
void cpl_system_assert_c_wrapper( bool true_expression, const char* file, unsigned line, const char* func );

#ifdef _cpluscplus
}
#endif

#endif  // end header latch

