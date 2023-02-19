#include "Bsp/Api.h"
#include "Cpl/Io/Serial/RP2040/Uart/InputOutput.h"
#include "pico/time.h"



extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

#define TX_FIFO_SIZE    128
#define RX_FIFO_SIZE    512

static uint8_t txFIFO_[TX_FIFO_SIZE];
static uint8_t rxFIFO_[RX_FIFO_SIZE];

static Cpl::Io::Serial::RP2040::Uart::InputOutput uartfd_( txFIFO_, sizeof( txFIFO_ ), rxFIFO_, sizeof( rxFIFO_ ) );
/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize the board
    Bsp_Api_initialize();

    // Initialize CPL
    //Cpl::System::Api::initialize();

    
    // Go run the test(s) (Note: This method should never return)
    uartfd_.start();    // Accepting the default UART Config parameters, e.g. 115200, 8N1
    test( uartfd_, uartfd_ );

    // I should never get here!
	for( ;; );
    return 0;
}

uint32_t elapsedTimeMs()
{
    return (uint32_t)(to_ms_since_boot(get_absolute_time()));
}

