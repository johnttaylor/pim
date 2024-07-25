#ifndef Cpl_Io_Serial_ST_M32F4_StreamDriver_h_
#define Cpl_Io_Serial_ST_M32F4_StreamDriver_h_
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

#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/Container/RingBuffer.h"
#include "Cpl/System/Thread.h"

/** Maximum number of UARTs/instances that are supported */
#ifndef OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS
#define OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS	    1
#endif


/// 
namespace Cpl {
/// 
namespace Io {
/// 
namespace Serial {
/// 
namespace ST {
///
namespace M32F4 {

/** This concrete class implements a non-busy-wait blocking Transmit/Receive Stream
    UART driver with a SOFTWARE FIFO.

    This driver ASSUMES that at most there is only ONE client (per TX/RX operations)
    attempting to use the driver at any given time.  The TX and RX clients can
    be different clients/threads.

    It is okay to call the start(), stop(), write() from  different threads -
    but the calls CANNOT be concurrent.  It is the application's  responsibility
    to provide additional thread-safety/concurrence protection.
 */
class StreamDriver
{
public:
    /** Constructor. Note: The driver does not begin operating until start() is
        called.
     */
    StreamDriver( Cpl::Container::RingBuffer<uint8_t>& txBuffer,
                  Cpl::Container::RingBuffer<uint8_t>& rxBuffer ) noexcept;

public:
    /** This method starts/enables the driver.  Note: The application is
        RESPONSIBLE for performing the low level initialization (Baud Rate,
        framing, etc.) of the UART BEFORE this method is called. For the
        application to change the Baud rate, framing, etc - it must first stop
        the driver and then restart it.
     */
    void start( IRQn_Type           uartIrqNum,
                UART_HandleTypeDef*	uartHdl ) noexcept;

    /** This method will stop/disable the driver.  The driver can be restarted
        by call start() again.  The state of the contents of the outbound buffer
        and the byte(s) 'in transmit' when the driver is stop is undetermined.
     */
    void stop( void ) noexcept;



public:
    /** Transmits the specified number of bytes.  The method does not return
        until all bytes have been transferred to the outbound buffer. The
        application CANNOT  assume that the byte(s) have been physically
        transmitted on the 'wire' when this method returns. The method returns
        true if successful; else false is returned  When an error is encounter
        there is no guaranty/information-available with  respect to how many (if
        any) bytes where transmitted.
     */
    bool write( const void* data, size_t numBytesToTx ) noexcept;


public:
    /** Receives at most the specified maximum number of bytes.  The actual
        number of bytes received is returned via 'numBytesRx'.  The method does
        not  return until at least one byte is available in the inbound buffer.
        The  method returns true if successful; else false is returned.

        NOTE: UART Framing errors are silents discarded, i.e. an incoming
              byte that is received with an associated framing error is
              NOT put into the inbound buffer.  A free running counter is
              maintain of the number of framing errors encountered.
     */
    bool read( void* data, int maxBytes, int& numBytesRx ) noexcept;


    /** This method returns true if at least one byte is available in the
        inbound buffer.
     */
    bool available( void ) const noexcept;


    /** This method returns and optionally clears the driver's RX error
        counter.
     */
    size_t getRXErrorsCounts( bool clearCount=true ) noexcept;

protected:
    /** This method SHOULD only be called when the HAL Transmit operation has
        completed (aka from the HAL_UART_TxCpltCallback() function).

        This method executes in the context of interrupt service routine (ISR)
     */
    void su_txDoneIsr( void ) noexcept;

    /** The method SHOULD only be called when the HAL Receive operation has 
        completed (aka from the HAL_UART_RxCpltCallback() or 
        HAL_UART_ErrorCallback() functions).

        This method executes in the context of interrupt service routine (ISR)
     */
    void su_rxDataAndErrorIsr( uint16_t bytesReceived ) noexcept;


protected:
    /** Used to pre-process the HAL_UART_TxCpltCallback() call to the
        specific Driver instance
     */
    static void su_txCompleteCallback( UART_HandleTypeDef* huart ) noexcept;

    /** Used to pre-process the HAL_UARTEx_RxEventCallback() call to the 
        specific Driver instance
     */
    static void su_rxEventCompleteCallback( UART_HandleTypeDef *huart, uint16_t bytesReceived ) noexcept;

    /** Used to pre-process the HAL_UART_ErrorCallback() call to the specific 
        Driver instance
     */
    static void su_rxErrorCompleteCallback( UART_HandleTypeDef* huart ) noexcept;

private:
    /// Prevent access to the copy constructor -->Driver can not be copied!
    StreamDriver( const StreamDriver& m );

    /// Prevent access to the assignment operator -->Driver can not be copied!
    const StreamDriver& operator=( const StreamDriver& m );


protected:
    /// Used to map a HAL UART Handle to an instance of this class
    struct HalMapping_T
    {
        UART_HandleTypeDef* halHandle;		//!< HAL Handle of the UART
        StreamDriver*		driver;			//!< Associated CPL driver instance
    };

protected:
    /// Handle to my low level hardware
    UART_HandleTypeDef*		m_uartHdl;

    /// Handle of the blocked TX client thread (if there is one)
    Cpl::System::Thread*    m_txWaiterPtr;

    /// Handle of the blocked RX client thread (if there is one)
    Cpl::System::Thread*    m_rxWaiterPtr;

    /// IRQ number of the UART being used
    IRQn_Type               m_uartIrqNum;

    /// Transmit buffer
    Cpl::Container::RingBuffer<uint8_t>& m_txBuffer;

    /// Receive buffer
    Cpl::Container::RingBuffer<uint8_t>& m_rxBuffer;

    /// A Receive error was encountered
    size_t                  m_errCount;

    /// Number of bytes requested to be transmitted by the HAL
    unsigned                m_txSize;

    /// Receive state
    bool                    m_rxActive;

    /// Map the HAL UART to a transmitter instance
    static HalMapping_T		m_mappings[OPTION_CPL_IO_SERIAL_ST_M32F4_MAX_UARTS];
};



}       // end namespaces
}
}
}
}
#endif  // end header latch
