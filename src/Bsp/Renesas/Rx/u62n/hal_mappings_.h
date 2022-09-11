#ifndef Bsp_Renesas_62n_hal_mappings_x_h_
#define Bsp_Renesas_62n_hal_mappings_x_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file 


    This file provides the LCONFIG mapping of various Cpl::Driver::xxxx Hardware
    abstraction interfaces to the Renesas RX62N microcontroller

    DO NOT include this file directly! Instead include the generic BSP
    interface - //colony.core/src/Bsp/Api.h.


*----------------------------------------------------------------------------*/ 

//////////////////////////////////////////////////////////////////////////////
// Driver::Uart

///
#define Driver_Uart_Hal_T_MAP   uint8_t


///
#define Driver_Uart_Hal_transmitByte_MAP        Bsp_uartTransmitByte
///
#define Driver_Uart_Hal_clrTxIrq_MAP(h)         // Empty because entering the ISR clears the IRQ request flag
///
#define Driver_Uart_Hal_enableTx_MAP            Bsp_uartEnableRX_TX     // Enable BOTH because that is the way the chip works!
///
#define Driver_Uart_Hal_disableTx_MAP           Bsp_uartDisableTX
///
#define Driver_Uart_Hal_enableTxIrq_MAP         Bsp_uartEnableTXDataIrq
///
#define Driver_Uart_Hal_disableTxIrq_MAP        Bsp_uartDisableTXDataIrq
///
#define Driver_Uart_Hal_isTxIrqEnabled_MAP      Bsp_uartIsTXDataIrqEnabled
///
#define Driver_Uart_Hal_isTxIrq_MAP             Bsp_uartIsTxDataEmpty
///
#define Driver_Uart_Hal_isTxPipeEmpty_MAP       Bsp_uartIsTxDataEmpty
///
#define Driver_Uart_Hal_resetTxPipe_MAP         Bsp_uartClrTxDataEmptyFlag  


///
#define Driver_Uart_Hal_getRxByte_MAP           Bsp_uartGetReceivedByte
///
#define Driver_Uart_Hal_clrRxIrq_MAP(h)         // Empty because entering the ISR clears the IRQ request flag    
///
#define Driver_Uart_Hal_isRxError_MAP           Bsp_uartIsReceiveError
///
#define Driver_Uart_Hal_clrRxErrors_MAP         Bsp_uartClrReceiveError
///
#define Driver_Uart_Hal_enableRx_MAP            Bsp_uartEnableRX_TX     // Enable BOTH because that is the way the chip works!
///
#define Driver_Uart_Hal_disableRx_MAP           Bsp_uartDisableRX
///
#define Driver_Uart_Hal_enableRxIrq_MAP         Bsp_uartEnableRXIrq
///
#define Driver_Uart_Hal_disableRxIrq_MAP        Bsp_uartDisableRXIrq
///
#define Driver_Uart_Hal_isRxIrq_MAP             Bsp_uartIsRxIrqEnabled


// Cpl::Driver::Uart
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
// Cpl::Io::Serial::Renesas::Rx62n

#define Cpl_Io_Serial_Renesas_Rx62n_Hal_uartInit_MAP    Bsp_uartInit

// Cpl::Io::Serial::Renesas::Rx62n
//////////////////////////////////////////////////////////////////////////////

#endif  
