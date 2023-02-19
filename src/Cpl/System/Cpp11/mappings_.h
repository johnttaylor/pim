#ifndef Cpl_System_Cpp11_mappings_x_h_
#define Cpl_System_Cpp11_mappings_x_h_
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

#include "colony_config.h"  // Note: Yet another work around for managing OS specific items not covered by C++11, e.g. the native directory separator


#include <mutex>
#include <condition_variable>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include "Cpl/System/FatalError.h"
#include <chrono>


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


/// C++11 Mapping
#define CPL_SYSTEM_ASSERT_MAP(e)                do { if ( !(e) ) Cpl::System::FatalError::logf( "ASSERT Failed at: file=%s, line=%d, func=%s\n", __FILE__, __LINE__, CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME ); } while(0)

/// C++11 Mapping
#define Cpl_System_Thread_NativeHdl_T_MAP       std::thread::native_handle_type

/// C++11 Mapping
#define Cpl_System_Mutex_T_MAP                  std::recursive_mutex

/// No native Semaphore in C++11 -->so build one out of a condition variable
typedef struct Cpl_System_Cpp11_Sema_Tag
{
    ///
    std::mutex              m_mutex;
    ///
    std::condition_variable m_cond;
    ///
    unsigned                m_count;
} Cpl_System_Cpp11_Sema_T;


/// C++11 Mapping
#define Cpl_System_Sema_T_MAP                   Cpl_System_Cpp11_Sema_T


/// C++11 Mapping
#define Cpl_System_TlsKey_T_MAP                 int

/// Mapping
inline unsigned long CplSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    return (unsigned long) std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
}

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_RAISE_MAP        0

/// Thread Priorities
#define CPL_SYSTEM_THREAD_PRIORITY_LOWER_MAP        0


/// Hack because Microsoft's VC12 only HAS PARTIAL support for thread_local
#if _MSC_VER >= 1800
#define  thread_local __declspec(thread)
#endif


#endif  // end header latch
