#include "pico/cyw43_arch.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Cpl/Io/Tcp/_0test/client.h"
#include "Cpl/Io/Tcp/lwIP/Picow/AsyncConnector.h"

#ifndef OPTION_PORT_NUM
#define OPTION_PORT_NUM  5002
#endif

static Cpl::Io::Tcp::lwIP::Picow::AsyncConnector connector_;

#define STOP()      for ( ;; )
/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();
    stdio_init_all();

    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "*Cpl::Io::Tcp" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );

    // Initialize WIFI Chip
    if ( cyw43_arch_init() ) 
    {
        CPL_SYSTEM_TRACE_MSG( "_0test", ("cyw43_arch_init() FAILED" ));
        STOP();
    }

    // Enable WIFI station mode
    cyw43_arch_enable_sta_mode();

    // Attempt to connect to the WIFI network
    CPL_SYSTEM_TRACE_MSG( "_0test", ( "Connecting to WiFi...") );
    if ( cyw43_arch_wifi_connect_timeout_ms( WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000 ) ) 
    {
        CPL_SYSTEM_TRACE_MSG( "_0test", ( "FAILED to connect to the WIFI network.") );
        STOP();
    }
    else 
    {
        CPL_SYSTEM_TRACE_MSG( "_0test", ("CONNECTED to the wife network: %s", WIFI_SSID) );
        CPL_SYSTEM_TRACE_MSG( "_0test", ( "My IP address: %s", ip4addr_ntoa( netif_ip4_addr( netif_list ) ) ));

    }

    // run the test
    runTest( connector_, OPTION_REMOTE_HOST_ADDRESS, OPTION_PORT_NUM );

    // I should never get here!
    STOP();
    return 1;
}


// These C++ stubs are expected to be part of bsp/ implementation for the 
// target board.  This test app does not use a 'BSP' - so the test code has
// to provide the implementation.
std::ios_base::Init::Init()
{
}

std::ios_base::Init::~Init()
{
}
