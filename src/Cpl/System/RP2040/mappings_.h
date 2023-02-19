#ifndef Cpl_System_RP2040_mappings_x_h_
#define Cpl_System_RP2040_mappings_x_h_
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

    This file provides the mappings for Platform specific types, etc. needed
    by Cpl::System

 */


#include <stdint.h>
#include <stdlib.h>
#include "colony_config.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/RP2040/mutex_t_.h"
#include "Cpl/System/RP2040/semaphore_t_.h"

/// PRETTY_FUNCTION macro is non-standard
#if defined(__GNUC__)
/// Take advantage of GCC's pretty function symbol
#define CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME    __PRETTY_FUNCTION__

#elif defined(_MSC_VER)
/// Take advantage of Microsoft's pretty function symbol
#define CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME    __FUNCSIG__

#else
/// Use C++11 function name
#define CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME    __func__
#endif  // end __PRETTY_FUNCTION__

/// Number of cores on the RP2040 MCU
#define CPL_SYSTEM_RP2040_NUM_CORES                 2


/// RP2040 Mapping
#define CPL_SYSTEM_ASSERT_MAP(e)                    do { if ( !(e) ) Cpl::System::FatalError::logf( "ASSERT Failed at: file=%s, line=%d, func=%s\n", __FILE__, __LINE__, CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME ); } while(0)

/// RP2040 Mapping
#define Cpl_System_Thread_NativeHdl_T_MAP           unsigned  

/// RP2040 Mapping
#define Cpl_System_Mutex_T_MAP                      Cpl::System::RP2040::Mutex_T

/// RP2040 Mapping
#define Cpl_System_Sema_T_MAP                       Cpl::System::RP2040::Semaphore_T

/// RP2040 Mapping
#define Cpl_System_TlsKey_T_MAP                     unsigned

/// Mapping
#define CPL_IO_NEW_LINE_NATIVE_MAP                  "\n"


/// Mapping
inline unsigned long CplSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    return to_ms_since_boot( get_absolute_time() );
}

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define CPL_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core) 
#define CPL_SYSTEM_THREAD_PRIORITY_RAISE_MAP        0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define CPL_SYSTEM_THREAD_PRIORITY_LOWER_MAP        0



#endif  // end header latch
