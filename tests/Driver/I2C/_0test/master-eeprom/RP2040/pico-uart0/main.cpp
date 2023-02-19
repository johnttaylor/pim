#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Driver/I2C/_0test/master_eeprom.h"
#include "Driver/I2C/RP2040/Master.h"


#ifndef OPTION_I2C_DEVICE
#define OPTION_I2C_DEVICE   i2c0
#endif

#ifndef OPTION_I2C_BAUDRATE
#define OPTION_I2C_BAUDRATE (100*1000)      // 100KHz
#endif

#ifndef OPTION_I2C_TIMEOUT
#define OPTION_I2C_TIMEOUT  (1000)          // 1 second
#endif

#ifndef OPTION_I2C_SDA_PIN
#define OPTION_I2C_SDA_PIN  PICO_DEFAULT_I2C_SDA_PIN    // GP4/pin.6
#endif

#ifndef OPTION_I2C_SCL_PIN
#define OPTION_I2C_SCL_PIN  PICO_DEFAULT_I2C_SCL_PIN    // GP5/pin.7
#endif

#ifndef OPTION_I2C_DEV_ADDRESS
#define OPTION_I2C_DEV_ADDRESS  0x50
#endif

static Driver::I2C::RP2040::Master uut( OPTION_I2C_DEVICE, OPTION_I2C_BAUDRATE, OPTION_I2C_TIMEOUT );

/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** UNIT TEST START-UP *****\n" );
    Cpl::System::RP2040::getConsoleStream().write( "Launching unit tests...\n" );
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );

    // Configure the I2C pins
    Driver::I2C::RP2040::Master::configureI2CPins( OPTION_I2C_SDA_PIN, OPTION_I2C_SCL_PIN );

    // Go run the test(s)
    runtests( uut, OPTION_I2C_DEV_ADDRESS );

    // I should never get here!
	for( ;; );
    return 0;
}
