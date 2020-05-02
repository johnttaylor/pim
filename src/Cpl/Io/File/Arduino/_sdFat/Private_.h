#ifndef Cpl_Io_File_Arduino_SdFat_Private_h_
#define Cpl_Io_File_Arduino_SdFat_Private_h_
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

    This file defines a global handle to the target's SdFat File system.  The
    target's BSP is RESPONSIBLE for initializing and creating (i.e. calling 
    .begin()) on the file system instance as part of the board initialization

 */


#include <SdFat.h>

extern FatFileSystem g_arduino_sdfat_fs;


#endif  // end header latch

