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

#include "InputOutput.h"
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/timer.h>
#include <hardware/regs/intctrl.h>
#include <pico/sync.h>

///
using namespace Cpl::Io::Serial::RP2040::Uart;

InputOutput* InputOutput::m_uart0Instance;
InputOutput* InputOutput::m_uart1Instance;

static critical_section_t lockUart0_;
static critical_section_t lockUart1_;

static inline uint8_t* advancePointer( uint8_t* ptrToAdvance, uint8_t* startOfMemPtr, size_t memSize )
{
    uint8_t* newPtr = ptrToAdvance + 1;
    if ( ((size_t) (newPtr - startOfMemPtr)) >= memSize )
    {
        newPtr = startOfMemPtr;
    }
    return newPtr;
}

////////////////////////////////////
InputOutput::InputOutput( uint8_t*      memTxBuffer,
                          size_t        txBufSize,
                          uint8_t*      memRxBuffer,
                          size_t        rxBufSize,
                          uart_inst_t*  uartHdl
)
    : m_uartHdl( uartHdl )
    , m_txBuffer( memTxBuffer )
    , m_txBufSize( txBufSize )
    , m_txHead( memTxBuffer )
    , m_txTail( memTxBuffer )
    , m_rxBuffer( memRxBuffer )
    , m_rxBufSize( rxBufSize )
    , m_rxHead( memRxBuffer )
    , m_rxTail( memRxBuffer )
    , m_started( false )
{
    if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART0 )
    {
        m_lock = &lockUart0_;
        critical_section_init( m_lock );
    }
    else if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART1 )
    {
        m_lock = &lockUart1_;
        critical_section_init( m_lock );
    }
}

InputOutput::~InputOutput( void )
{
    stop();
}

void InputOutput::start( unsigned long baudRate,
                         unsigned      txPin,
                         unsigned      rxPin,
                         unsigned      dataBits,
                         unsigned      stopBits,
                         uart_parity_t parity )
{
    // Ignore if already started
    if ( !m_started )
    {
        // Initialize the hardware
        m_started = true;
        m_txHead  = m_txBuffer;
        m_txTail  = m_txBuffer;
        m_rxHead  = m_rxBuffer;
        m_rxTail  = m_rxBuffer;
        uart_init( m_uartHdl, baudRate );
        uart_set_format( m_uartHdl, dataBits, stopBits, parity );
        gpio_set_function( txPin, GPIO_FUNC_UART );
        gpio_set_function( rxPin, GPIO_FUNC_UART );
        uart_set_fifo_enabled( m_uartHdl, true );
        uart_set_hw_flow( m_uartHdl, false, false );  // Disable HW flow control

        // Set up IRQ Handlers
        if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART0 )
        {
            m_uart0Instance = this;
            irq_clear( UART0_IRQ );
            irq_set_exclusive_handler( UART0_IRQ, uart0IrqHandler );
            irq_set_enabled( UART0_IRQ, true );
        }
        else if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART1 )
        {
            m_uart1Instance = this;
            irq_clear( UART1_IRQ );
            irq_set_exclusive_handler( UART1_IRQ, uart1IrqHandler );
            irq_set_enabled( UART1_IRQ, true );
        }
        uart_set_irq_enables( m_uartHdl, true, false );
    }
}

void InputOutput::stop()
{
    // Ignore if already stopped
    if ( m_started )
    {
        // Disable the RX/TX interrupts
        m_started = false;
        uart_set_irq_enables( m_uartHdl, false, false );

        if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART0 )
        {
            irq_set_enabled( UART0_IRQ, false );
            irq_remove_handler( UART0_IRQ, uart0IrqHandler );
            m_uart0Instance = 0;
        }
        else if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART1 )
        {
            irq_set_enabled( UART1_IRQ, false );
            irq_remove_handler( UART1_IRQ, uart1IrqHandler );
            m_uart1Instance = 0;
        }

        uart_deinit( m_uartHdl );
    }
}


////////////////////////////////////
void InputOutput::uart0IrqHandler( void )
{
    if ( m_uart0Instance )
    {
        m_uart0Instance->irqHandler();
    }
}

/// ISR handler for UART0
void InputOutput::uart1IrqHandler( void )
{
    if ( m_uart1Instance )
    {
        m_uart1Instance->irqHandler();
    }
}

void InputOutput::irqHandler()
{
    // Get the IRQ flags
    uint32_t irqFlags = uart_get_hw( m_uartHdl )->mis;

    // Receive IRQ
    if ( irqFlags & (UART_UARTMIS_RXMIS_BITS | UART_UARTMIS_RTMIS_BITS) )
    {
        // Need to use a critical section - because we CANNOT guaranteed that 
        // the core where read/write is being called is the same core as where 
        // the ISR run
        critical_section_enter_blocking( m_lock );

        // Drain the HW Receive FIFO
        while ( uart_is_readable( m_uartHdl ) )
        {
            // Read the incoming byte
            uint8_t c = (uint8_t) uart_getc( m_uartHdl );

            // Calculate a new head pointer (handling rolling over at the end of the buffer space)
            uint8_t* newRxHead = advancePointer( m_rxHead, m_rxBuffer, m_rxBufSize );

            // Discard the incoming byte if the SW FIFO is full
            if ( newRxHead != m_rxTail )
            {
                *m_rxHead = c;
                m_rxHead  = newRxHead;
            }
        }

        // Exit the critical section
        critical_section_exit( m_lock );
    }

    // Transmit IRQ
    if ( irqFlags & (UART_UARTMIS_TXMIS_BITS) )
    {
        // See previous comment about why a Critical section is needed in the ISR
        critical_section_enter_blocking( m_lock );

        // Fill the HW Transmit FIFO
        fillHwTxFifo();

        // Disable TX IRQ for now if we have nothing left to transmit
        if ( m_txTail == m_txHead )
        {
            uart_set_irq_enables( m_uartHdl, true, false );
        }

        // Exit the critical section
        critical_section_exit( m_lock );
    }
}

void InputOutput::fillHwTxFifo()
{
    while ( uart_is_writable( m_uartHdl ) && m_txTail != m_txHead )
    {
        // Write the outgoing byte
        uart_putc_raw( m_uartHdl, (char) *m_txTail );

        // Calculate a new tail pointer (handling rolling over at the end of the buffer space)
        m_txTail = advancePointer( m_txTail, m_txBuffer, m_txBufSize );
    }
}

////////////////////////////////////
bool InputOutput::available()
{
    if ( !m_started )
    {
        return false;
    }

    critical_section_enter_blocking( m_lock );
    bool avail = m_rxHead != m_rxTail;
    critical_section_exit( m_lock );
    return avail;
}

bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    bytesRead       = 0;
    uint8_t* dstPtr = (uint8_t*) buffer;

    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Ignore read of zero bytes
    if ( numBytes == 0 )
    {
        return true;
    }

    // Drain the SW RX FIFO
    critical_section_enter_blocking( m_lock );
    while ( numBytes && m_rxHead != m_rxTail )
    {
        // Get the next available byte
        *dstPtr++ = *m_rxTail;
        numBytes--;
        bytesRead++;

        // Calculate a new tail pointer (handling rolling over at the end of the buffer space)
        m_rxTail = advancePointer( m_rxTail, m_rxBuffer, m_rxBufSize );
    }
    critical_section_exit( m_lock );

    // If I get here, the read operation succeeded
    return true;
}

bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    bytesWritten          = 0;
    const uint8_t* srcPtr = (const uint8_t*) buffer;

    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Ignore write of zero bytes
    if ( maxBytes == 0 )
    {
        return true;
    }

    // Fill the SW TX FIFO
    critical_section_enter_blocking( m_lock );
    while ( maxBytes )
    {
        // Calculate a new head pointer (handling rolling over at the end of the buffer space)
        uint8_t* newTxHead = advancePointer( m_txHead, m_txBuffer, m_txBufSize );

        // Stop if the SW TX FIFO is full
        if ( newTxHead == m_txTail )
        {
            break;
        }

        // Add to the SW TX FIFO
        *m_txHead = *srcPtr++;
        m_txHead  = newTxHead;
        maxBytes--;
        bytesWritten++;
    }

    // Trigger a transmit if the Transmitter has gone had gone idle
    uart_set_irq_enables( m_uartHdl, true, true );
    fillHwTxFifo();
    critical_section_exit( m_lock );

    // If I get here, the write operation succeeded
    return true;
}


void InputOutput::flush()
{
    // At best the SW TX FIFO could be 'flushed' to the HW TX FIFO, but that 
    // doesn't guarantee the data was physically transmitted. So the
    // choice was made to do nothing.
}

bool InputOutput::isEos()
{
    // Does not really have meaning for serial port (assuming the serial port is opened/active)
    return false;
}

void InputOutput::close()
{
    // Does not really have meaning for serial port
    flush();
}

