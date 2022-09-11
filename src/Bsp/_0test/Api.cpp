/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2018  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/


#include "Bsp/Api.h"
#include "Bsp/Execution.h"
#include "Bsp/WatchDog.h"
#include <stdio.h>


/// Stub
#define Bsp_Api_nop_MAP()

/// Stub
#define Bsp_Api_disableIrqs_MAP()

/// Stub
#define Bsp_Api_enableIrqs_MAP()

/// Stub
#define Bsp_Api_pushAndDisableIrqs_MAP()

/// Stub
#define Bsp_Api_popIrqs_MAP()

/// Stub
#define Bsp_Api_turnOn_debug1_MAP()

/// Stub
#define Bsp_Api_turnOff_debug1_MAP()

/// Stub
#define Bsp_Api_toggle_debug1_MAP()

/// Stub
#define Bsp_Api_turnOn_debug2_MAP()

/// Stub
#define Bsp_Api_turnOff_debug2_MAP()

/// Stub
#define Bsp_Api_toggle_debug2_MAP()


/// Stub
void Bsp_Api_initialize( void )
{
    Bsp_Api_nop();
    Bsp_Api_disableIrqs();
    Bsp_Api_enableIrqs();
    Bsp_Api_pushAndDisableIrqs();
    Bsp_Api_popIrqs();
    Bsp_Api_turnOn_debug1();
    Bsp_Api_turnOff_debug1();
    Bsp_Api_toggle_debug1();
    Bsp_Api_turnOn_debug2();
    Bsp_Api_turnOff_debug2();
    Bsp_Api_toggle_debug2();

    int r = -1;
    switch ( Bsp_Execution_getResetType() )
    {
        case BSP_EXECUTION_RESET_UNKNOWN:   r = BSP_EXECUTION_RESET_UNKNOWN;   break;
        case BSP_EXECUTION_RESET_COLD:      r = BSP_EXECUTION_RESET_COLD;      break;
        case BSP_EXECUTION_RESET_WARM:      r = BSP_EXECUTION_RESET_WARM;      break;
        case BSP_EXECUTION_RESET_BROWN_OUT: r = BSP_EXECUTION_RESET_BROWN_OUT; break;
        case BSP_EXECUTION_RESET_EXECPTION: r = BSP_EXECUTION_RESET_EXECPTION; break;
        default: break;
    }

    printf( "reset type=%d\n", r );
    Bsp_Execution_reboot();
    Bsp_Execution_shutdown();
}
