#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Driver/NV/_0test/test.h"
#include "Driver/NV/Onsemi/CAT24C512/Api.h"
#include "Driver/I2C/STM32/Master.h"
#include "Cpl/System/FreeRTOS/Thread.h"



#ifndef OPTION_I2C_DEV_ADDRESS
#define OPTION_I2C_DEV_ADDRESS              (0x50)
#endif

static Driver::I2C::STM32::Master           i2cDriver_( &hi2c2 );
static Driver::NV::Onsemi::CAT24C512::Api   uut_( i2cDriver_, OPTION_I2C_DEV_ADDRESS );

#define SECT_   "_0test"

/// Thread for the test (note: the console driver only works when called from a CPL thread)
class ThreadMain : public Cpl::System::Runnable
{
public:
    ThreadMain() {};

protected:
    void appRun()
    {
        i2cDriver_.start(); // Start the low-level driver (because the NV Driver does not start it since it is shareable resource)
        runtests( uut_,
                  OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES,
                  OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE,
                  OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES * OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE
        );
        i2cDriver_.stop();
    }
};

/* NOTE: The way FreeRTOS start the 'first' task - corrupts the raw main
         stack -->this means we cannot allocate anything on the raw main
         stack and expect it to stay in scope for duration of the application
*/
static ThreadMain runnable_;

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("**** DRIVER TEST APPLICATION STARTED ****") );

    //// Create the main thread
    Cpl::System::Thread* t1 = Cpl::System::FreeRTOS::Thread::create( runnable_, "main", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
    if ( t1 == nullptr )
    {
        printf( "Failed to create the main thread\n" );
        while ( 1 );
    }

    // Start the scheduler
    Cpl::System::Api::enableScheduling(); // Enable scheduling NEVER return!
    return 0;
}
