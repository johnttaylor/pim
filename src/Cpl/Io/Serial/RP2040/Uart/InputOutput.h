#ifndef Cpl_Io_Serial_RP2040_Uart_InputOutput_h_
#define Cpl_Io_Serial_RP2040_Uart_InputOutput_h_
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
/** @file */

#include "Cpl/Io/InputOutput.h"
#include "Bsp/Api.h"    // For UART defaults
#include "hardware/uart.h"
#include <stdint.h>

/// Handle for UART0
#define CPL_IO_SERIAL_RP2040_UART_HANDLE_UART0  uart0

/// Handle for UART1
#define CPL_IO_SERIAL_RP2040_UART_HANDLE_UART1  uart1


///
namespace Cpl {
///
namespace Io {
///
namespace Serial {
///
namespace RP2040 {
///
namespace Uart {


/** This concrete class implements a Input stream using Raspberry Pi UART
    interface/hardware. This implement provide a more robust stream interface
    than using 'stdio_uart' because it is interrupted driven and provides
    software TX/RX FIFOs

    Notes:
        1. The UART Interrupt service handlers execute on the core that 
           executing start() method.
        2. The implement does NOT support the blocking semantics of the Cpl::Io
           streams.  The design decision was for the implementation to be
           compatible/useful on bare-metal systems, i.e. no threads required.

 */

class InputOutput : public Cpl::Io::InputOutput
{
public:
    /// Constructor.
    InputOutput( uint8_t*      memTxBuffer,        //!< Memory for the transmit buffer
                 size_t        txBufSize,          //!< Size, in bytes, of the transmit buffer   
                 uint8_t*      memRxBuffer,        //!< Memory for the receive buffer
                 size_t        rxBufSize,          //!< Size, in bytes, of the receive buffer   
                 uart_inst_t*  uartHdl  = BSP_DEFAULT_UART_HANDLE   //!< Which UART to use
               );

    /// Destructor
    ~InputOutput( void );

public:
    /// This method is used to start the stream/driver
    void start( unsigned long baudRate  = 115200,                       //!< Baud rate in hertz
                unsigned      txPin     = BSP_DEFAULT_UART_TX_PIN,      //!< Transmit Pin
                unsigned      rxPin     = BSP_DEFAULT_UART_RX_PIN,      //!< Receive Pin
                unsigned      dataBits  = 8,                            //!< Number of data bits.  Range is [5..8]
                unsigned      stopBits  = 1,                            //!< Number of stop bits.  Range is [1..2]
                uart_parity_t parity    = UART_PARITY_NONE              //!< Parity setting.  See hardware/uart.h for enumeration
    );

    /// This method is used to stop the stream/driver. The stream can be restarted after is has been stopped
    void stop();

public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::read;

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();


public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::write;

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output
    void flush();

    /// See Cpl::Io::IsEos. Note: This method always returns false
    bool isEos();

    /// See Cpl::Io::Output
    void close();

protected:
    /// ISR handler for UART0
    static void uart0IrqHandler( void );

    /// ISR handler for UART0
    static void uart1IrqHandler( void );

    /// Common ISR handler (does most of the ISR work)
    void irqHandler();

    /// Helper method to fill the HW TX FIFO
    void fillHwTxFifo();

protected:
    /// UART Handle
    uart_inst_t*        m_uartHdl;

    /// Memory for the transmit buffer
    uint8_t*            m_txBuffer;
    
    /// Size, in bytes, of the transmit buffer 
    size_t              m_txBufSize;
    
    /// Tail pointer for TX FIFO
    uint8_t*            m_txHead;

    /// Head pointer for TX FIFO
    uint8_t*            m_txTail;

    /// Memory for the receive buffer
    uint8_t*            m_rxBuffer;
    
    /// Size, in bytes, of the receive buffer 
    size_t              m_rxBufSize;

    /// Tail pointer for RX FIFO
    uint8_t*            m_rxHead;

    /// Head pointer for RX FIFO
    uint8_t*            m_rxTail;

    /// Pointer to UART0 instance (if there is one)
    static InputOutput* m_uart0Instance;

    /// Pointer to UART1 instance (if there is one)
    static InputOutput* m_uart1Instance;

    /// Critical section for ISR/thread/Core exclusion
    critical_section_t* m_lock;

    /// My started/stopped state
    bool                m_started;
};

};      // end namespaces
};
};
};
};
#endif  // end header latch
