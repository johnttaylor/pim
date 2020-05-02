#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Driver/Uart/Hal.h"


extern void loopback_test( Driver_Uart_Hal_T uartHdl, bool manualFirstTx );

// UNIT TEST -->Only checks if the code compiles & links using different compilers

int main( int argc, char* argv[] )
{
	// Initialize the board
	Bsp_Api_initialize();

	// Initialize CPL
	Cpl::System::Api::initialize();

	// Go run the test(s) (Note: This method should never return)
	loopback_test( 0, true );

	// Return 'pass'
	return 0;
}



