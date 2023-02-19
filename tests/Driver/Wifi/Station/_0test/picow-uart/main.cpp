#include "pico/cyw43_arch.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Cpl/Io/Tcp/lwIP/Picow/AsyncListener.h"
#include "Driver/Wifi/Station/_0test/test.h"
#include "Bsp/Api.h"

#ifndef OPTION_PORT_NUM
#define OPTION_PORT_NUM  5002
#endif

static Cpl::Io::Tcp::lwIP::Picow::AsyncListener connector_;

#define STOP()      for ( ;; )
/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board (note: initializes the CYW34 WIFI chip driver)
    Bsp_Api_initialize();

    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "*Driver::Wifi::Station" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );

    // run the test
    runTest( Cpl::System::RP2040::getConsoleStream(),
             Cpl::System::RP2040::getConsoleStream(),
             connector_, 
             OPTION_PORT_NUM );

    // I should never get here!
    STOP();
    return 1;
}

