#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Driver/Uart/Stream/Transmitter.h"
#include "Driver/Uart/Stream/Receiver.h"
#include "Cpl/Io/Null.h"
#include <stdlib.h>


extern void loopback_test( Driver_Uart_Hal_T uartHdl, bool manualFirstTx );

extern Driver::Uart::Stream::Transmitter* txPtr;
extern Driver::Uart::Stream::Receiver*    rxPtr;

static Cpl::Io::Null fd_;

/*-----------------------------------------------------------*/
int main( void )
{
	// Initialize the board
	Bsp_Api_initialize();
	Bsp_uartInit( BSP_DB9_CONSOLE, BSP_DB9_CONSOLE_PIN_SELECT, configMAX_SYSCALL_INTERRUPT_PRIORITY - 2, BSP_BAUDRATE_57600, BSP_BAUDRATE_DIVIDER_57600, ( BSP_PARITY_NONE | BSP_DATABITS_8 | BSP_STOPBITS_1 ) );

	// Initialize CPL
	Cpl::System::Api::initialize();

	// Go run the test(s) (Note: This method should never return)
	loopback_test( BSP_DB9_CONSOLE, true );

	// I should never get here!
	for ( ;; );
	return 0;
}


/*-----------------------------------------------------------*/
Cpl::Io::Output* Cpl::System::Trace::getDefaultOutputStream_( void ) throw( )
{
	return &fd_;
}

/*-----------------------------------------------------------*/
void consoleTxISR( void )
{
	static BaseType_t xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = txPtr->su_txDoneIsr_();

	/* If xHigherPriorityTaskWoken was set to true you
	we should yield.  The actual macro used here is
	port specific. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void consoleRxISR( void )
{
	static BaseType_t xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = rxPtr->su_rxDataAndErrorIsr_();

	/* If xHigherPriorityTaskWoken was set to true you
	we should yield.  The actual macro used here is
	port specific. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
