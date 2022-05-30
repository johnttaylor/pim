#ifndef Bsp_driver_hal_mappings_x_h_
#define Bsp_driver_hal_mappings_x_h_
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


    This file provides the LCONFIG mapping of various Cpl::Driver::xxxx Hardware
    abstraction interfaces to a null/empty implementation

    DO NOT include this file directly! Instead include the generic BSP
    interface - //colony.bsp/src/Bsp/Api.h.


*----------------------------------------------------------------------------*/ 

//////////////////////////////////////////////////////////////////////////////
// Driver::Uart

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_T_MAP                   uint8_t


/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_transmitByte_MAP(h,b)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_clrTxIrq_MAP(h) 

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_enableTx_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_disableTx_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_enableTxIrq_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_disableTxIrq_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_isTxIrqEnabled_MAP(h)   (0)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_isTxIrq_MAP(h)          (0)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_isTxPipeEmpty_MAP(h)    (0)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_resetTxPipe_MAP(h)



/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_getRxByte_MAP(h)        (0)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_clrRxIrq_MAP(h)     

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_isRxError_MAP(h)        (0)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_clrRxErrors_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_enableRx_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_disableRx_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_enableRxIrq_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_disableRxIrq_MAP(h)

/// User space HAL mapping for: Driver::Uart
#define Driver_Uart_Hal_isRxIrq_MAP(h)          (0)


// Cpl::Driver::Uart
//////////////////////////////////////////////////////////////////////////////




#endif  
