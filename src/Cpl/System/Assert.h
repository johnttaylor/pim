#ifndef Cpl_System_Assert_h_
#define Cpl_System_Assert_h_
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

#include "colony_config.h"
#include "colony_map.h"

#ifdef USE_CPL_SYSTEM_ASSERT_MACROS

/** This macro is platform/compiler independent replacement for 'assert()'
    function/macro.  If the passed boolean expression is false (i.e. zero)
    then the application is terminated.  The specific actions that occur when
    the application is terminated is platform specific.  The 'signature' for
    the macro/function is:

    \b Prototype: void CPL_SYSTEM_ASSERT( <boolean expression> );

    Note: This assert macro must be "turned on" at compile time by defining
          the USE_CPL_SYSTEM_ASSERT_MACROS symbol.
 */
#define CPL_SYSTEM_ASSERT                   CPL_SYSTEM_ASSERT_MAP

#else

/// Empty macro
#define CPL_SYSTEM_ASSERT(e)

#endif  // end USE_CPL_SYSTEM_ASSERT_MACROS


#endif  // end header latch

