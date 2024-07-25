#include "Bsp/Api.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/console/Output.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Driver/DIO/_0test/_out_pwm/out_pwm.h"

static DriverDioOutSTM32PinConfig_T pin_( GPOUT_Test_Pin_GPIO_Port, GPOUT_Test_Pin_Pin );
static Driver::DIO::Out             uutOut_( pin_ , true );

static DriverDioPwmSTM32Config_T timerPwm_( UUT_PWM_TIMER_BLOCK_PTR, UUT_PWM_TIMER_CHANNEL);
static Driver::DIO::Pwm          uutPwm_( timerPwm_ );

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
    runtests( uutOut_, uutPwm_, 1 );

    // I should never get here!
    for ( ;; );
    return 0;
}
