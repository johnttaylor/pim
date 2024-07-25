
#include "Bsp/Api.h"
#include <stdio.h>
#include <stdint.h>
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/FreeRTOS/Thread.h"

class Thread1 : public Cpl::System::Runnable
{
public:
    Thread1() {};

protected:
    void appRun()
    {
        Cpl::System::Thread& t1 = Cpl::System::Thread::getCurrent();
        printf( "Thread Running: %s, %x, %p\n", t1.getName(), t1.getId(), t1.getNativeHandle() );
 
        uint32_t timeMark1 = Cpl::System::ElapsedTime::milliseconds();
        uint32_t timeMark2 = timeMark1;
        for ( ;;)
        {
            uint32_t now = Cpl::System::ElapsedTime::milliseconds();
            if ( now - timeMark1 >= 100 )
            {
                Bsp_Api_toggle_debug1();
                timeMark1 = now;
            }
            if ( now - timeMark2 >= 1000 )
            {
                Bsp_Api_toggle_debug2();
                timeMark2 = now;
                printf( "now=%lu\n", now );
            }

            uint32_t now2 = Cpl::System::ElapsedTime::milliseconds();
            Cpl::System::Api::sleep( 10 - ( now2 - now ) );
        }
    }
};

/* NOTE: The way FreeRTOS start the 'first' task - corrupts the raw main
         stack -->this means we cannot allocate anything on the raw main
         stack and expect it to stay in scope for duration of the applicatoin
*/
static Thread1 runnable_;

int main( void )
{
    // Initialize the board
    Bsp_Api_initialize();
    printf( "\n**** BSP TEST APPLICATION STARTED ****\n\n" );

    //// Initialize CPL
    Cpl::System::Api::initialize();

    //// Create my test thread
    Cpl::System::Thread* t1 = Cpl::System::FreeRTOS::Thread::create( runnable_, "TEST", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
    if ( t1 == nullptr )
    {
        printf( "Failed to created thread!!\n" );
        while ( 1 );
    }
    else
    {
        printf( "Thread created %s thread\n", t1->getName() );
    }

    //// Start the scheduler
    Cpl::System::Api::enableScheduling();
    while ( 1 );    // Enable scheduling should NEVER return!
}


