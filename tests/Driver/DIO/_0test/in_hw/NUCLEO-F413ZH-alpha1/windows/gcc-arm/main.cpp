#include "Bsp/Api.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/console/Output.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Driver/DIO/_0test/_in_hw/in.h"

static DriverDioInSTM32PinConfig_T pin_( GPIN_TEST_PIN_GPIO_Port, GPIN_TEST_PIN_Pin );
//static DriverDioInSTM32PinConfig_T pin_( HW_SAFETY_GPIO_Port, HW_SAFETY_Pin );
static Driver::DIO::In             uutIn_( pin_ , false );

#define SECT_   "_0test"

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board
    Bsp_Api_initialize();

    // Initialize CPL
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("**** DRIVER TEST APPLICATION STARTED ****") );

    // Go run the test(s) (Note: This method should never return)
    runtests( uutIn_, 500 );

    // I should never get here!
    for ( ;; );
    return 0;
}
