#include "Bsp/Api.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/console/Output.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/FreeRTOS/Thread.h"
#include "Driver/AIO/HalSingleInput.h"
#include "Driver/AIO/_0test/singleinput.h"

#define SECT_   "_0test"

/// Thread for the test (note: the console driver only works when called from a CPL thread)
class ThreadMain : public Cpl::System::Runnable
{
public:
    ThreadMain() {};

protected:
    void appRun()
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("**** DRIVER TEST APPLICATION STARTED ****") );
        Driver_AIO_HalSingle_setADCSize( &hadc1, 12 );
        runtests( &hadc1 );   // This method never returns
    }
};

/* NOTE: The way FreeRTOS start the 'first' task - corrupts the raw main
         stack -->this means we cannot allocate anything on the raw main
         stack and expect it to stay in scope for duration of the applicatoin
*/
static ThreadMain runnable_;

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board
    Bsp_Api_initialize();

    // Initialize CPL
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );

    // Create the main thread
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
