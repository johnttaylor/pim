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

#include "StreamDriver.h"
#include "Cpl/System/api.h"
#include "Cpl/System/FatalError.h"

/// 
using namespace Cpl::Io::Serial::ST::M32F4;

StreamDriver::HalMapping_T Cpl::Io::Serial::ST::M32F4::StreamDriver::m_mappings[OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS] = { 0, };

#define ENTER_CRITICAL_SECTION()    do { Cpl::System::Api::suspendScheduling(); Bsp_NVIC_disableIRQ( m_uartIrqNum ); } while(0)
#define EXIT_CRITICAL_SECTION()     do { Cpl::System::Api::resumeScheduling(); Bsp_NVIC_enableIRQ( m_uartIrqNum ); } while (0)

////////////////////////
StreamDriver::StreamDriver( Cpl::Container::RingBuffer<uint8_t>& txBuffer,
                            Cpl::Container::RingBuffer<uint8_t>& rxBuffer ) noexcept
    : m_uartHdl( nullptr )
    , m_txWaiterPtr( nullptr )
    , m_rxWaiterPtr( nullptr )
    , m_txBuffer( txBuffer )
    , m_rxBuffer( rxBuffer )
    , m_errCount( 0 )
{
    // generating 1st half of the HAL/Driver mapping (basically it 'allocates' a slot in the look-up table for the instance)
    bool mapped = false;
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].driver == nullptr )
        {
            m_mappings[idx].driver = this;
            mapped                 = true;
            break;
        }
    }
    if ( !mapped )
    {
        Cpl::System::FatalError::logf( "Cpl::Io::Serial::ST::M32F4::StreamDriver. Too many driver instances." );
    }
}


////////////////////////
void StreamDriver::start( IRQn_Type             uartIrqNum,
                          UART_HandleTypeDef*	uartHdl ) noexcept
{
    // If in the wrong state -->get in the correct state
    if ( m_uartHdl != nullptr )
    {
        stop();
    }

    // finish the HAL/Driver mapping
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].driver == this )
        {
            m_mappings[idx].halHandle = uartHdl;
            break;
        }
    }

    // Housekeeping
    m_uartHdl    = uartHdl;
    m_uartIrqNum = uartIrqNum;
    m_errCount   = 0;
    m_txBuffer.clearTheBuffer();
    m_rxBuffer.clearTheBuffer();

    // Register ISR callbacks
    HAL_UART_RegisterCallback( m_uartHdl, HAL_UART_TX_COMPLETE_CB_ID, su_txCompleteCallback );
    HAL_UART_RegisterRxEventCallback( m_uartHdl, su_rxEventCompleteCallback );
    HAL_UART_RegisterCallback( m_uartHdl, HAL_UART_ERROR_CB_ID, su_rxErrorCompleteCallback );

    // The HAL expects a 'flat' buffer (not a ring buffer) - so we have to
    // extract the 'flat portion' of ring buffer to pass to the HAL.
    unsigned availBytes = 0;
    uint8_t* rxPtr      = m_rxBuffer.peekNextAddItems( availBytes );

    // Start the receiver
    m_rxActive = true;
    HAL_UARTEx_ReceiveToIdle_IT( m_uartHdl, rxPtr, availBytes );
}

void StreamDriver::stop( void ) noexcept
{
    // Stop any in-progress transactions
    HAL_UART_Abort_IT( m_uartHdl );

    // Unregister ISR callbacks
    HAL_UART_UnRegisterCallback( m_uartHdl, HAL_UART_TX_COMPLETE_CB_ID );
    HAL_UART_UnRegisterRxEventCallback( m_uartHdl );
    HAL_UART_UnRegisterCallback( m_uartHdl, HAL_UART_ERROR_CB_ID );

    // Remove HAL/driver mapping
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].driver == this )
        {
            m_mappings[idx].halHandle = nullptr;
            break;
        }
    }

    // Free up any blocked clients (if there are any)
    if ( m_txWaiterPtr )
    {
        m_txWaiterPtr->signal();
    }
    if ( m_rxWaiterPtr )
    {
        m_rxWaiterPtr->signal();
    }

    // Housekeeping
    m_txWaiterPtr = nullptr;
    m_rxWaiterPtr = nullptr;
    m_uartHdl     = nullptr;
}



////////////////////////
bool StreamDriver::write( const void* data, size_t numBytesToTx ) noexcept
{
    // Fail the write if the driver has not been started
    if ( m_uartHdl == nullptr )
    {
        return false;
    }

    // Do nothing if no data needs to be transmitted
    if ( numBytesToTx == 0 )
    {
        return true;
    }

    // Loop until all data has been transferred to the outbound buffer
    uint8_t* ptr = (uint8_t*) data;
    for ( ;;)
    {
        // Transfer my data to the outbound buffer
        while ( numBytesToTx )
        {
            // Populate Ring Buffer
            bool result = m_txBuffer.add( *ptr );

            // Get the next byte if my Ring Buffer was not full
            if ( result )
            {
                ptr++;
                numBytesToTx--;
            }

            // Ring Buffer is full -->skip to transmitting
            else
            {
                break;
            }
        }

        // If not all my outbound data fits in the buffer -->I will block
        if ( numBytesToTx )
        {
            Cpl::System::Thread& myThread = Cpl::System::Thread::getCurrent();

            // INTERRUPT/CRITICAL SECTION: Set Waiter
            ENTER_CRITICAL_SECTION();
            m_txWaiterPtr = &myThread;
            EXIT_CRITICAL_SECTION();
        }

        // Do nothing if there is a 'transmit in progress' 
        if ( ( HAL_UART_GetState( m_uartHdl ) & HAL_UART_STATE_BUSY_TX ) != HAL_UART_STATE_BUSY_TX )
        {
            // The HAL expects a 'flat' buffer (not a ring buffer) - so we have to
            // extract the 'flat portion' of ring buffer to pass to the HAL.
            ENTER_CRITICAL_SECTION();
            uint8_t* txPtr = m_txBuffer.peekNextRemoveItems( m_txSize );

            // Start transmitting 
            if ( txPtr )
            {
                HAL_UART_Transmit_IT( m_uartHdl, txPtr, m_txSize );
            }
            EXIT_CRITICAL_SECTION();    // NOTE: The Critical section MUST include the call to HAL_UART_Transmit_IT()
        }

        // Wait (if necessary) for buffer to be transmitted/drained
        if ( numBytesToTx )
        {
            Cpl::System::Thread::wait();
        }

        // All done -->exit outer loop
        else
        {
            break;
        }
    }

    return true;
}

void StreamDriver::su_txDoneIsr( void ) noexcept
{
    // Remove the transmitted bytes from TX FIFO and check if there still more data to transmit
    m_txBuffer.removeElements( m_txSize );
    uint8_t* txPtr = m_txBuffer.peekNextRemoveItems( m_txSize );

    // Start a new transmit if the TX FIFO is NOT empty
    if ( txPtr != nullptr )
    {
        // Start transmitting 
        HAL_UART_Transmit_IT( m_uartHdl, txPtr, m_txSize );
    }

    // By definition if I get here - there is space available in the TX FIFO.
    // Unblock waiting client (if there is one) now that there is space in the FIFO
    if ( m_txWaiterPtr )
    {
        int result    = m_txWaiterPtr->su_signal();
        m_txWaiterPtr = nullptr;
        Bsp_yieldOnExit( result );
    }
}

///////////////////////////////////////////////////////////////////////////////

bool StreamDriver::available( void ) const noexcept
{
    if ( m_uartHdl != nullptr )
    {
        ENTER_CRITICAL_SECTION();
        unsigned count = m_rxBuffer.getNumItems();
        EXIT_CRITICAL_SECTION();

        return count > 0;
    }

    return false;
}


size_t StreamDriver::getRXErrorsCounts( bool clearCount ) noexcept
{
    // INTERRUPT/CRITICAL SECTION: Get RX Framing Error counts
    ENTER_CRITICAL_SECTION();
    size_t count = m_errCount;
    if ( clearCount )
    {
        m_errCount = 0;
    }
    EXIT_CRITICAL_SECTION();

    return count;
}


bool StreamDriver::read( void* data, int maxBytes, int& numBytesRx ) noexcept
{
    numBytesRx = 0;

    // Return an error if the drive has not yet been started
    if ( m_uartHdl == nullptr )
    {
        return false;
    }

    // Do nothing if the app is requesting zero bytes to read
    if ( maxBytes == 0 )
    {
        return true;
    }

    // Block if there is no data available
    // NOTE: A RX Error has the potential to unblock the reader WHILE the RX FIFO is still empty
    Cpl::System::Thread& myThread = Cpl::System::Thread::getCurrent();
    for ( ;;)
    {
        ENTER_CRITICAL_SECTION();
        if ( m_rxBuffer.getNumItems() == 0 )
        {
            m_rxWaiterPtr = &myThread;
            EXIT_CRITICAL_SECTION();

            // Wait for incoming byte
            Cpl::System::Thread::wait();
        }
        else
        {
            EXIT_CRITICAL_SECTION();
            break;
        }
    }

    // Housekeeping
    uint8_t  byteIn;
    uint8_t* ptr = (uint8_t*) data;

    // Read as much a possible (at this point there is at least one byte in the inbound buffer)
    while ( maxBytes )
    {
        // Get the next byte from the inbound buffer
        bool result = m_rxBuffer.remove( byteIn );

        // Copy the inbound byte to the client's data buffer
        if ( result )
        {
            *ptr++ = byteIn;
            numBytesRx++;
            maxBytes--;

            // NOW that there is some room in the FIFO - restart the Receiver if it was disabled due to the FIFO being full
            ENTER_CRITICAL_SECTION();
            if ( m_rxActive == false )
            {
                unsigned availBytes = 0;
                m_rxActive          = true;
                uint8_t* rxPtr      = m_rxBuffer.peekNextAddItems( availBytes );

                HAL_UARTEx_ReceiveToIdle_IT( m_uartHdl, rxPtr, availBytes );
                EXIT_CRITICAL_SECTION();        // Note: The Critical section MUST include the call to HAL_UARTEx_ReceiveToIdle_IT()
            }
            else
            {
                EXIT_CRITICAL_SECTION();
            }
        }

        // No data (or no more data) is available -->exit the loop
        else
        {
            break;
        }
    }


    // Always return success
    return true;
}

void StreamDriver::su_rxDataAndErrorIsr( uint16_t bytesReceived ) noexcept
{
    // Clear any/all RX Error Flags
    if ( __HAL_UART_GET_FLAG( m_uartHdl,
         ( UART_FLAG_PE | UART_FLAG_FE | UART_FLAG_NE | UART_FLAG_ORE ) ) != RESET )
    {
        __HAL_UART_CLEAR_FLAG( m_uartHdl, ( UART_FLAG_PE | UART_FLAG_FE | UART_FLAG_NE | UART_FLAG_ORE ) );
        m_errCount++;
    }


    // Update the FIFO's meta data for the received data
    m_rxBuffer.addElements( bytesReceived );

    // unblock waiting client (if there is one) now that I have least one byte
    if ( m_rxWaiterPtr )
    {
        int result    = m_rxWaiterPtr->su_signal();
        m_rxWaiterPtr = nullptr;
        Bsp_yieldOnExit( result );
    }

    // Restart the Receiver if there is space in the RX FIFO
    if ( !m_rxBuffer.isFull() )
    {
        unsigned availBytes = 0;
        m_rxActive          = true;
        uint8_t* rxPtr      = m_rxBuffer.peekNextAddItems( availBytes );
        HAL_UARTEx_ReceiveToIdle_IT( m_uartHdl, rxPtr, availBytes );
    }
    else
    {
        m_rxActive = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void StreamDriver::su_txCompleteCallback( UART_HandleTypeDef* huart ) noexcept
{
    // Look-up the driver instance from the HAL handle
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].halHandle == huart && m_mappings[idx].driver != nullptr )
        {
            m_mappings[idx].driver->su_txDoneIsr();
            break;
        }
    }
}

void StreamDriver::su_rxErrorCompleteCallback( UART_HandleTypeDef* huart ) noexcept
{
    // Look-up the driver instance from the HAL handle
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].halHandle == huart && m_mappings[idx].driver != nullptr )
        {
            m_mappings[idx].driver->su_rxDataAndErrorIsr( 0 );
            return;
        }
    }
}

void StreamDriver::su_rxEventCompleteCallback( UART_HandleTypeDef *huart, uint16_t bytesReceived )
{
    // Look-up the driver instance from the HAL handle
    for ( unsigned idx=0; idx < OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS; idx++ )
    {
        if ( m_mappings[idx].halHandle == huart && m_mappings[idx].driver != nullptr )
        {
            m_mappings[idx].driver->su_rxDataAndErrorIsr( bytesReceived );
            return;
        }
    }
}
