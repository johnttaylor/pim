#ifndef Driver_Uart_Hal_h_
#define Driver_Uart_Hal_h_
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
    
    This file defines a hardware abstraction layer (HAL) for accessing a
    hardware register based UART.

	NOTE: NO 'Initialization' method is provided/defined in this interface -
          this is intentional! The initialization of the baud rate, number of
          start/stop bits, etc. is VERY platform specific - which translate
          to very cumbersome/inefficiencies in trying to make a generic one
          size-fits-all init() method. What does this mean.... well first the
          application is RESPONSIBLE for making sure that the platform specific
          initialization happens BEFORE any of the Cpl drivers are started.  
          Second, this interface ASSUMES that the required 'Uart Handle' is 
          returned/created (at least conceptually) from this platform specific 
          init() routine.
*/

#include <stdint.h>
#include "colony_map.h"


/*-------------- PUBLIC API ------------------------------------------------*/
/** This data type defines the platform specific 'handle' of a UART.  The
    'handle' is used to unique identify a specific UART instance.
 */
#define Driver_Uart_Hal_T               Driver_Uart_Hal_T_MAP



/*-------------- PUBLIC API ------------------------------------------------*/
/** This method places the specified byte in to the transmit data register and
    initiates a transmit sequence.

    Prototype:
        void Driver_Uart_Hal_transmitByte( Driver_Uart_Hal_T hdl, uint8_t byteToTransmit );
 */
#define  Driver_Uart_Hal_transmitByte         Driver_Uart_Hal_transmitByte_MAP


/** This method clears the Tx Interrupt request.  This method can be empty/null
    if the loading the data register clears the IRQ.

    Prototype:
        void Driver_Uart_Hal_clrTxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_clrTxIrq             Driver_Uart_Hal_clrTxIrq_MAP


/** This method enables the UART transmitter 

    Prototype:
        void Driver_Uart_Hal_enableTx( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_enableTx             Driver_Uart_Hal_enableTx_MAP


/** This method disables the UART transmitter 

    Prototype:
        void Driver_Uart_Hal_disableTx( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_disableTx            Driver_Uart_Hal_disableTx_MAP


/** This method enables the UART Transmit Data register emtpy interrupt 

    Prototype:
        void Driver_Uart_Hal_enableTxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_enableTxIrq          Driver_Uart_Hal_enableTxIrq_MAP


/** This method disables the UART Transmit Data register empty interrupt
    Prototype:
        void Driver_Uart_Hal_enableTxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_disableTxIrq         Driver_Uart_Hal_disableTxIrq_MAP


/** This method returns true if the Transmit Data empty interrupt is enabled
    Prototype:
        bool Driver_Uart_Hal_isTxIrqEnabled( Driver_Uart_Hal_T hdl );
 */
#define Driver_Uart_Hal_isTxIrqEnabled        Driver_Uart_Hal_isTxIrqEnabled_MAP

       
/** This method return non-zero ( Driver_Uart_Hal_T hdl, true) if the 'current' UART interrupt
    request is a tx-data-register-empty interrupt.

    Prototype:
        bool Driver_Uart_Hal_isTxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_isTxIrq              Driver_Uart_Hal_isTxIrq_MAP


/** This method returns non-zero ( Driver_Uart_Hal_T hdl, true) if the UART transmit buffer
    register is empty.  This method is used to 'manually' load the first 
    transmit byte.  

    Prototype
        bool Driver_Uart_Hal_isTxPipeEmpty( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_isTxPipeEmpty         Driver_Uart_Hal_isTxPipeEmpty_MAP


/** This method is used to reset/prep the UART's transmit engine for
    the first byte transmit condition. 

    Prototype
        void Driver_Uart_Hal_resetTxPipe( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_resetTxPipe           Driver_Uart_Hal_resetTxPipe_MAP



/*-------------- PUBLIC API ------------------------------------------------*/
/** This method returns the last received byte, independent of any Rx errors

    Prototype:
        uint8_t Driver_Uart_Hal_getRxByte( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_getRxByte            Driver_Uart_Hal_getRxByte_MAP


/** This method clears the Rx Interrupt request.  This method can be empty/null
    if the reading the received byte clears the IRQ.

    Prototype:
        void Driver_Uart_Hal_clrRxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_clrRxIrq             Driver_Uart_Hal_clrRxIrq_MAP


/** This method returns non-zero (true) if any Rx error (framing, overrung, 
    parity, etc.) has occurred on the 'current' byte 

    Prototype:
        uint8_t Driver_Uart_Hal_isRxError( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_isRxError            Driver_Uart_Hal_isRxError_MAP


/** This method clears ALL Rx errors for the 'current' byte.

    Prototype:
        void Driver_Uart_Hal_clrRxErrors( Driver_Uart_Hal_T hdl );
*/
#define  Driver_Uart_Hal_clrRxErrors          Driver_Uart_Hal_clrRxErrors_MAP


/** This method enables the UART receiver 

    Prototype:
        void Driver_Uart_Hal_enableRx( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_enableRx             Driver_Uart_Hal_enableRx_MAP


/** This method disables the UART receiver 

    Prototype:
        void Driver_Uart_Hal_disableRx( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_disableRx            Driver_Uart_Hal_disableRx_MAP


/** This method enables the UART Receive data interrupt.

    Prototype:
        void Driver_Uart_Hal_enableRxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_enableRxIrq          Driver_Uart_Hal_enableRxIrq_MAP


/** This method disables the UART Receive data interrupt.

    Prototype:
        void Driver_Uart_Hal_disableRxIrq( Driver_Uart_Hal_T hdl );
 */
#define  Driver_Uart_Hal_disableRxIrq         Driver_Uart_Hal_disableRxIrq_MAP


/** This method return non-zero (true) if the 'current' UART interrupt
    request is a unread-data-in-the-rx-data-registe interrupt.

    Prototype:
        bool Driver_Uart_Hal_isRxIrq( Driver_Uart_Hal_T hdl );
 */ 
#define  Driver_Uart_Hal_isRxIrq              Driver_Uart_Hal_isRxIrq_MAP




/*--------------------------------------------------------------------------*/
#endif  
