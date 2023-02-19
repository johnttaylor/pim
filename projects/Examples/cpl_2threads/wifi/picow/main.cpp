#include "../../../../application.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Io/Tcp/AsyncListener.h"
#include "Cpl/Io/Tcp/lwIP/Picow/AsyncListener.h"
#include "pico/cyw43_arch.h"


#ifndef OPTION_PORT_NUM
#define OPTION_PORT_NUM  5002
#endif


/// Allocate memory for the console stream pointers
Cpl::Io::Input*  g_consoleInputFdPtr;
Cpl::Io::Output* g_consoleOutputFdPtr;

Cpl::Text::FString<128> tmpString_;

// IO Stream over a WIFI connection
class ListenerClient : public Cpl::Io::Tcp::AsyncListener::Client
{
public:
    ListenerClient() {}
    bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo ) noexcept
    {
        Cpl::System::RP2040::getConsoleStream().write( tmpString_, "Accepted incoming connection. Remote Host: %s\n", rawConnectionInfo );
        activate( newFd );
        return true;
    }
};

static Cpl::Io::Tcp::lwIP::Picow::AsyncListener listener_;
static ListenerClient                           wifiFd_;

/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    
    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );
    Cpl::System::RP2040::getConsoleStream().write( "Cpl and the Bsp have been initialized.\n" );

    // Enable WIFI station mode
    cyw43_arch_enable_sta_mode();

    // Attempt to connect to the WIFI network
    CPL_SYSTEM_TRACE_MSG( "_0test", ("Connecting to WiFi...\n") );
    if ( cyw43_arch_wifi_connect_timeout_ms( WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000 ) )
    {
        Cpl::System::RP2040::getConsoleStream().write("FAILED to connect to the WIFI network.");
    }
    else
    {
        Cpl::System::RP2040::getConsoleStream().write( tmpString_, "CONNECTED to the wife network: %s\n", WIFI_SSID );
        Cpl::System::RP2040::getConsoleStream().write( tmpString_, "My IP address: %s\n", ip4addr_ntoa( netif_ip4_addr( netif_list ) ) );
    }

    // Set the stream for the console
    g_consoleInputFdPtr  = &wifiFd_;
    g_consoleOutputFdPtr = &wifiFd_;

    // Start the application
    runApplication();         // This method should never return
    return 0;
}

void platformHook_core0_beginThread()
{
    // NOT USED
}
void platformHook_core1_beginThread()
{
    listener_.startListening( wifiFd_, OPTION_PORT_NUM );

}
void platformHook_core1_idleThread()
{
    listener_.poll();
}
