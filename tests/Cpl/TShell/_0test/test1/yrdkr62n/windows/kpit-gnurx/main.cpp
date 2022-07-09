#include "Bsp/Api.h"
#include "Cpl/Io/Serial/Renesas/Rx62n/InputOutput.h"               
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/TShell/Cmd/FreeRTOS/Threads.h"
#include <stdlib.h>


/// Create my InputOutput stream for Shell and Trace logging
static Cpl::Io::Serial::Renesas::Rx62n::InputOutput<256,32> fd_( BSP_DB9_CONSOLE, BSP_DB9_CONSOLE_PIN_SELECT, configMAX_SYSCALL_INTERRUPT_PRIORITY-2, BSP_BAUDRATE_57600, BSP_BAUDRATE_DIVIDER_57600, (BSP_PARITY_NONE|BSP_DATABITS_8|BSP_STOPBITS_1) );


// External references
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Container::Map<Cpl::TShell::Command>  cmdlist("ignore_this_parameter-used to invoke the static constructor" );
Cpl::TShell::Cmd::FreeRTOS::Threads        threadsCmd_( cmdlist );


/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize the board
    Bsp_Api_initialize();

    // Initialize Colony
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

	// Run the test
    fd_.start();
    shell_test( fd_, fd_ );

    // do not exit
    for(;;)
        {
        Cpl::System::Api::sleep(1000);
        }

	return 0;
}
    

/*-----------------------------------------------------------*/
Cpl::Io::Output* Cpl::System::Trace::getDefaultOutputStream_(void) throw()
	{
	return &fd_;
	}

               
/*-----------------------------------------------------------*/
void consoleTxISR( void )
	{
    static BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = fd_.su_getTransmitter_().su_txDoneIsr_();

    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}


void consoleRxISR( void )
	{
    static BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = fd_.su_getReceiver_().su_rxDataAndErrorIsr_();

    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
