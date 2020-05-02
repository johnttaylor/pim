#ifndef Cpl_System_BareMetal_win32_mappings_x_h_
#define Cpl_System_BareMetal_win32_mappings_x_h_
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
/** @file

    This file provides the mappings for Platform specific types, etc. needed
    by Cpl::System

 */

#include "Cpl/System/BareMetal/mappings_.h"
#include <windows.h>


/// Win32 Mapping
#define CPL_SYSTEM_SHELL_NULL_DEVICE_x_MAP      "NUL"

/// Win32 Mapping
#define CPL_SYSTEM_SHELL_SUPPORTED_x_MAP        1


/// Win32 Mapping
#define CPL_IO_NEW_LINE_NATIVE_MAP              "\015\012"

/// Win32 Mapping
#define CPL_IO_FILE_NATIVE_DIR_SEP_MAP          '\\'

/// Win32 Mapping
#define CPL_IO_FILE_MAX_NAME_MAP                _MAX_PATH

#endif  // end header latch
