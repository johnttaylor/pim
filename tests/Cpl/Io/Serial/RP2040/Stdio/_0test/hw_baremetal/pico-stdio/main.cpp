#include "Bsp/Api.h"
#include "Cpl/Io/Serial/RP2040/Stdio/Input.h"
#include "Cpl/Io/Serial/RP2040/Stdio/Output.h"
#include "pico/time.h"

/// Create my streams
static Cpl::Io::Serial::RP2040::Stdio::Input  infd_;
static Cpl::Io::Serial::RP2040::Stdio::Output outfd_;



extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );
    
/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize the board
    Bsp_Api_initialize();

    // Initialize CPL
    //Cpl::System::Api::initialize();

    
    // Go run the test(s) (Note: This method should never return)
    test( infd_, outfd_ );

    // I should never get here!
	for( ;; );
    return 0;
}

uint32_t elapsedTimeMs()
{
    return (uint32_t)(to_ms_since_boot(get_absolute_time()));
}

