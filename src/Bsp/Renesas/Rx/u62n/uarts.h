#ifndef Bsp_Renesas_62n_uarts_h_
#define Bsp_Renesas_62n_uarts_h_
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


    This file provides a basic interface for the RX62N's UARTs (i.e. SCIn 
    peripherals running in asynchronous mode).

    NOTE: This interface is NOT THREAD SAFE.  It it left to application
          to manage/handle/implement thread safety.


    DO NOT include this file directly! Instead include the generic BSP
    interface - //colony.core/src/Bsp/Api.h.

*----------------------------------------------------------------------------*/ 


#include "Bsp/Api.h"
#include <stdint.h>


// Start C++
#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////
/// Options
//////////////////////////

/** This value provide a 'wait time' to let UART control setting settle before
    performing other operations.  The number has NO units and is unused as 
    the loop count for a busy wait loop
 */
#ifndef OPTION_BSP_UARTS_WAIT_INTERVAL
#define OPTION_BSP_UARTS_WAIT_INTERVAL  10000
#endif


//////////////////////////
/// Serial Ports
//////////////////////////

/*
** Serial Port Identifiers (aka base addresses for SCI peripheral)
*/

/// Serial Port ID
#define BSP_ID_SERIAL_PORT0         0

/// Serial Port ID
#define BSP_ID_SERIAL_PORT1         1

/// Serial Port ID
#define BSP_ID_SERIAL_PORT2         2

/// Serial Port ID
#define BSP_ID_SERIAL_PORT3         3

/// Serial Port ID
#define BSP_ID_SERIAL_PORT5         5

/// Serial Port ID
#define BSP_ID_SERIAL_PORT6         6

/// Pin Selection/Configuration option 'A'
#define BSP_SERIAL_PORT_PINSEL_A    0

/// Pin Selection/Configuration option 'B'
#define BSP_SERIAL_PORT_PINSEL_B    1


/// Base address for SCI ports
#define BSP_BASE_ADDR_SCI_PORTS         (&(SCI0))

/// Macro converts port index to an SCI instance
#define BSP_SCI_INDEX_TO_INSTANCE(i)    (*(BSP_BASE_ADDR_SCI_PORTS + (i)))
//#define BSP_SCI_INDEX_TO_INSTANCE(i)        (*(volatile struct st_sci *) (0x88240+(i)*8))



/*
** Baud rate values used in UART initialization. Based on 48MHz PCLK (i.e. 12MHz oscillator * 4 ) 
*/

/// Baudrate
#ifndef BSP_BAUDRATE_1200
#define BSP_BAUDRATE_1200                   77                                                              
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_1200       
#define BSP_BAUDRATE_DIVIDER_1200           2
#endif
      
/// Baudrate
#ifndef BSP_BAUDRATE_2400
#define BSP_BAUDRATE_2400                   155                                                              
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_2400       
#define BSP_BAUDRATE_DIVIDER_2400           1
#endif
      
/// Baudrate
#ifndef BSP_BAUDRATE_4800
#define BSP_BAUDRATE_4800                   77                                                              
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_4800       
#define BSP_BAUDRATE_DIVIDER_4800           1
#endif
      
/// Baudrate
#ifndef BSP_BAUDRATE_9600
#define BSP_BAUDRATE_9600                   38                                                               
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_9600       
#define BSP_BAUDRATE_DIVIDER_9600           1
#endif
      
/// Baudrate
#ifndef BSP_BAUDRATE_19200 
#define BSP_BAUDRATE_19200                  77                                                               
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_19200       
#define BSP_BAUDRATE_DIVIDER_19200          0
#endif
      
/// Baudrate
#ifndef BSP_BAUDRATE_38400 
#define BSP_BAUDRATE_38400                  38 
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_38400       
#define BSP_BAUDRATE_DIVIDER_38400          0
#endif
      
/// Baudrate
#ifndef BSP_BAUDRATE_57600 
#define BSP_BAUDRATE_57600                  25 
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_57600       
#define BSP_BAUDRATE_DIVIDER_57600          0
#endif

/// Baudrate
#ifndef BSP_BAUDRATE_115200 
#define BSP_BAUDRATE_115200                 12
#endif
/// PCLK divider 
#ifndef BSP_BAUDRATE_DIVIDER_115200       
#define BSP_BAUDRATE_DIVIDER_115200         0
#endif


                                                                                                             
/* 
** Parity values used in UART initialization 
*/                                                              
/// Parity option
#define BSP_PARITY_NONE                     BSP_SMR_PAR_DISABLE_                                              

/// Parity option
#define BSP_PARITY_EVEN                     (BSP_SMR_PAR_ENABLE_|BSP_SMR_PAR_EVEN_)                            

/// Parity option
#define BSP_PARITY_ODD                      (BSP_SMR_PAR_ENABLE_|BSP_SMR_PAR_ODD_)                             

                                                                                                             

/* 
** Stop bits values used in UART initialization 
*/

/// Number of Stopbits option
#define BSP_STOPBITS_1                      BSP_SMR_1_STOP_                                                   

/// Number of Stopbits option
#define BSP_STOPBITS_2                      BSP_SMR_2_STOP_                                                   


/* 
** Data bits values used in UART initialization 
*/

/// Number of databits option
#define BSP_DATABITS_7                      BSP_SMR_7_CHAR_                                                   

/// Number of databits option
#define BSP_DATABITS_8                      BSP_SMR_8_CHAR_                                                   
 
                                                                                                             
/* 
** COMPONENT Scoped Serial mode register (SMR) bit values 
*/                                                                  
/// SMR bit position
#define BSP_SMR_PAR_ENABLE_                 0x20                                                          

/// SMR bit position
#define BSP_SMR_PAR_DISABLE_                0x00                                                          

/// SMR bit position
#define BSP_SMR_PAR_ODD_                    0x10                                                          

/// SMR bit position
#define BSP_SMR_PAR_EVEN_                   0x00                                                          

/// SMR bit position
#define BSP_SMR_1_STOP_                     0x00                                                          

/// SMR bit position
#define BSP_SMR_2_STOP_                     0x08 

/// SMR bit position
#define BSP_SMR_7_CHAR_                     0x40 

/// SMR bit position
#define BSP_SMR_8_CHAR_                     0x00                                                          
                                                                                                             

/* 
** Receive Status/Error Codes 
*/    
                                                                         
/// Bit mask: TX Data register empty flag
#define BSP_TX_DATA_EMPTY                   0x80   

/// Bit mask: TX done flag (all data+stopbits tranmitted)
#define BSP_TX_END                          0x04         

/// Bit mask: A byte has been received                                                 
#define BSP_RX_OK_RCVD_BYTE                 0x40                                                             

/// Bit mask: RX overrun error
#define BSP_RX_ERR_OVERRUN                  0x20                                                             

/// Bit mask: RX framing error
#define BSP_RX_ERR_FRAMING                  0x10                                                             

/// Bit mask: RX parity error
#define BSP_RX_ERR_PARITY                   0x08                                                              

/// Bit mask: RX errors
#define BSP_RX_ERR_INPUT                    (BSP_RX_ERR_OVERRUN|BSP_RX_ERR_FRAMING|BSP_RX_ERR_PARITY)        



/*------------------ PUBLIC API -----------------------------------------*/
/** This method provides a general purpose intialization routine
    for the board's UART.  
    
    @param sciPortID        UART/SCI Port ID (must be one of BSP_ID_SERIAL_PORTx)
    @param pinSelect        Pin select, i.e. Pin configuration 'A' vs 'B'.  Not all pin configuration options are available on all packages of the microcontroller
    @param baudrate         BRR value
    @param baudrateDivider  PCLK divider/prescaler. 0= PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/11
    @param irqPriority      Interrupt Priority. 1=lowest, ... 15=highest
    @param frameConfig      is set by bit-wise ORing the Parity, Stopbits, and data length values together

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks if 'sciPortID' is
             a valid ID.
 */
uint8_t Bsp_uartInit( uint8_t sciPortID, uint8_t pinSelect, uint8_t irqPriority, uint8_t baudrate, uint8_t baudrateDivider, uint8_t frameConfig );


/** This method returns true (non-zero) if there is received
    character in the Receive data register. 
 
    \b Prototype:
        uint8_t Bsp_uartIsByteReceived( uint8_t sciPortID );   
 */
#define Bsp_uartIsByteReceived(p)           (BSP_SCI_INDEX_TO_INSTANCE(p).SSR.BYTE & BSP_RX_OK_RCVD_BYTE)


/** This method EXPLICITY clears the receive interrupt request.  NOTE: Typically 
    this method is NOT needed because entering ISR clears the interrupt request 
    flag.

    @param sciPortID        UART/SCI Port ID (must be one of BSP_ID_SERIAL_PORTx)

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks if 'sciPortID' is
             a valid ID.
 */
uint8_t Bsp_uartClrReceivedFlag( uint8_t sciPortID );


/** This method returns true (non-zero) if there is receive
    error.
 */
#define Bsp_uartIsReceiveError(p)           (BSP_SCI_INDEX_TO_INSTANCE(p).SSR.BYTE & BSP_RX_ERR_INPUT)


/** This method is used to clear all (if any) receive errors
 */
#define Bsp_uartClrReceiveError(p)          BSP_SCI_INDEX_TO_INSTANCE(p).SSR.BYTE &= ~BSP_RX_ERR_INPUT


/** This method returns the received-byte status.  If there
    is no receive byte and/or error, then the method returns 0.  If a receive
    byte/error occurred, then the status/error code(s) are returned.  The 
    error code(s) are returned as bit-wised OR'd values.
 */
#define Bsp_uartGetReceiveStatus(p)         (BSP_SCI_INDEX_TO_INSTANCE(p).SSR.BYTE&(BSP_RX_OK_RCVD_BYTE|BSP_RX_ERR_INPUT))


/** This method returns the last byte received
 */
#define Bsp_uartGetReceivedByte(p)          BSP_SCI_INDEX_TO_INSTANCE(p).RDR


/** This method requests that the specified byte be transmitted (i.e.
    it loads the TX data register and starts the TX process) 
 */
#define Bsp_uartTransmitByte(p,b)            BSP_SCI_INDEX_TO_INSTANCE(p).TDR=(b)

 
/** This method returns true (non-zero) if the last requested byte 
    to be transmitted has been shifted out of the TX Data register
    into the TX Shift register.
 */
#define Bsp_uartIsTxDataEmpty(p)            (BSP_SCI_INDEX_TO_INSTANCE(p).SSR.BYTE& BSP_TX_DATA_EMPTY)


/** This method EXPLICITY clears the TX data done interrupt request.  
    NOTE: Typically this method is NOT needed because entering ISR clears the 
    interrupt request flag.

    @param sciPortID        UART/SCI Port ID (must be one of BSP_ID_SERIAL_PORTx)

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks if 'sciPortID' is
             a valid ID.
 */
uint8_t Bsp_uartClrTxDataEmptyFlag( uint8_t sciPortID );


/** This method returns true (non-zero) if the last bit of
    the last byte transmitted has actually been transmitted
    on the 'wire'
 */
#define Bsp_uartIsTxDone(p)                 (BSP_SCI_INDEX_TO_INSTANCE(p).SSR.BIT.TEND)   


/** This method enables transmit mode for the UART.  
    
    Notes: 
        1) If the transmitter is enable AFTER the TX Interrupt Enable flag has 
           been set, a TX DATA interrupt will be generated.
        2) The recommendation is to do NOT use this method - but use
           Bsp_uartEnableRX_TX() instead.  This method can only be used
           if the applicaiton ONLY transmits on the UART.
 */
#define Bsp_uartEnableTX(p)                 BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TE=1


/** This method disables transmit mode for the UART
 */
#define Bsp_uartDisableTX(p)                BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TE=0


/** This method enables receive mode for the UART.

    Notes: 
        1) The recommendation is to do NOT use this method - but use
           Bsp_uartEnableRX_TX() instead.  This method can only be used
           if the applicaiton ONLY receives on the UART.
 */
#define Bsp_uartEnableRX(p)                 BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.RE=1      


/** This method enables RX & TX at the same time. NOTE: The 62N effectively does not
    allow enabling RX & TX at different times.
 */
#define Bsp_uartEnableRX_TX(p)              BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BYTE |= 0x30


/** This method disables receive mode for the UART 
 */
#define Bsp_uartDisableRX(p)                BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.RE=0  


/** This method enables the Transmit Byte Interrupt for the UART 
    (the transmit data register is empty) 
 */
#define Bsp_uartEnableTXDataIrq(p)          BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TIE=1      


/** This method disables the Transmit Byte Interrupt for the UART 
 */
#define Bsp_uartDisableTXDataIrq(p)         BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TIE=0       


/** This method return non-zero value if the Transmit Byte Done Interrupt is
    enabled.
 */
#define Bsp_uartIsTXDataIrqEnabled(p)       (BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TIE==1)


/** This method return true (non-zero) if the Transmit Byte Interrupt
    for the UART is enabled
 */
#define Bsp_uartIsTxDataIrqEnabled(p)       (BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TIE)      


/** This method enables the Received Byte Interrupt for the UART 
 */
#define Bsp_uartEnableRXIrq(p)              BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.RIE=1      


/** This method disables the Received Byte Interrupt for the UART 
 */
#define Bsp_uartDisableRXIrq(p)             BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.RIE=0      


/** This method return true (non-zero) if the Receive Byte Interrupt
    for the UART is enabled
 */
#define Bsp_uartIsRxIrqEnabled(p)           (BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.RIE)      


/** This method enables the Transmit Completed Interrupt for the UART 
    (the transmit shift register is empty) 
 */
#define Bsp_uartEnableTXDoneIrq(p)          BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TEIE=1      


/** This method disables the Transmit Completed Interrupt for the UART 
 */
#define Bsp_uartDisableTXDoneIrq(p)         BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TEIE=0      


/** This method return true (non-zero) if the Transmit Completed Interrupt
    for the UART is enabled
 */
#define Bsp_uartIsTxDoneIrqEnabled(p)       (BSP_SCI_INDEX_TO_INSTANCE(p).SCR.BIT.TEIE)      



/** Helper method that imposes a busy wait.  Need to when new control settings need
    to 'settle' before moving on.  Typically the applicaiton should NOT call this
    method.
 */
void Bsp_uartBusyWait( unsigned long delay );


// END C++
#ifdef __cplusplus
};
#endif
#endif  // end header latch
