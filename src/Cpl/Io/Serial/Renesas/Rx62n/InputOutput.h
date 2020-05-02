#ifndef Cpl_Io_Serial_Renesas_Rx62n_InputOutput_h_
#define Cpl_Io_Serial_Renesas_Rx62n_InputOutput_h_
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
/** @file */

#include "Driver/Uart/Stream/InputOutput_.h"
#include "Cpl/Io/Serial/Renesas/Rx62n/Hal.h"


/// 
namespace  Cpl {
/// 
namespace Io {
/// 
namespace Serial {
/// 
namespace Renesas {
/// 
namespace Rx62n {


/** This template provides a concrete implementation of the Cpl::Io InputOutput
	stream interface using one of the native UART (SCIn) ports on the Renesas
	RX62n micro-controller.

	NOTE: The instance has to be 'started' before it can be used.  This allows the
		  instances to be created statically and then started/initialized after
		  the BSP and Cpl library has been initialized.

	Template Args:
		TXSIZE:=    Size, in bytes, for the TX buffer
		RXSIZE:=    Size, in bytes, for the RX buffer
 */
template <int TXSIZE, int RXSIZE>
class InputOutput : public Driver::Uart::Stream::InputOutput_
{
private:
	/// SCI Port selection
	uint8_t     m_sciPortID;

	/// Pin selection (A vs. B)
	uint8_t     m_pinSelect;

	/// IRQ Priority
	uint8_t     m_irqPriority;

	/// Baudrate
	uint8_t     m_baudrate;

	/// Baudrate divider
	uint8_t     m_baudrateDivider;

	/// Frame Configuration
	uint8_t     m_frameConfig;

	/// Raw memory for the TX buffer
	uint8_t     m_txMemBuffer[TXSIZE];

	/// Raw memory for the RX buffer
	uint8_t     m_rxMemBuffer[RXSIZE];


public:
	/** Constructor.  Note: The application must call start() before using
		the created instance.

		@param sciPortID        UART/SCI Port ID (must be one of {0,1,2,3,5,6}
		@param pinSelect        Pin select (A:=0, B:=1), i.e. Pin configuration 'A' vs 'B'.  Not all pin configuration options are available on all packages of the microcontroller
		@param baudrate         BRR value
		@param baudrateDivider  PCLK divider/prescaler. 0= PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/11
		@param irqPriority      Interrupt Priority. 1=lowest, ... 15=highest
		@param frameConfig      is set by bit-wise ORing the Parity, Stopbits, and data length values together

	 */
	InputOutput( Driver_Uart_Hal_T sciPortID,
		uint8_t           pinSelect,
		uint8_t           irqPriority,
		uint8_t           baudrate,
		uint8_t           baudrateDivider,
		uint8_t           frameConfig ) noexcept
		:Driver::Uart::Stream::InputOutput_( sciPortID, true, m_txMemBuffer, TXSIZE, m_rxMemBuffer, RXSIZE ),
		m_sciPortID( sciPortID ),
		m_pinSelect( pinSelect ),
		m_irqPriority( irqPriority ),
		m_baudrate( baudrate ),
		m_baudrateDivider( baudrateDivider ),
		m_frameConfig( frameConfig )
	{
	}


	/// Destructor
	~InputOutput() {}


public:
	/** This method must be called BEFORE using the InputOutput stream - BUT
		it must called AFTER the BSP and the CPL library has been initialized.
	 */
	void start( void ) noexcept
	{
		Cpl_Io_Serial_Renesas_Rx62n_Hal_uartInit( m_sciPortID, m_pinSelect, m_irqPriority, m_baudrate, m_baudrateDivider, m_frameConfig );
		m_tx.start();
		m_rx.start();
	}


public:
	/// This COMPONENT Scoped method returns the transmitter for the PURPOSE of processing the TX ISR
	Driver::Uart::Stream::Transmitter& su_getTransmitter_( void ) noexcept { return m_tx; }

	/// This COMPONENT Scoped method returns the receiver for the PURPOSE of processing the RX ISRs
	Driver::Uart::Stream::Receiver&    su_getReceiver_( void ) noexcept { return m_rx; }

};



};      // end namespaces
};
};
};
};
#endif  // end header latch
