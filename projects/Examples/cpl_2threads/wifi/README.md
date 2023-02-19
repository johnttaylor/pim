# Command Shell over WIFI

This variant of the thread example project is built for the PICOW and uses a TCP
connection over WIFI for the application's command shell.  The TCP communications 
is implemented using the `Cpl::Io::Tcp` non-blocking sockets built on top of 
[lwIP's](https://savannah.nongnu.org/projects/lwip/) RAW TCP interfaces.  The 
non-blocking sockets are compatible with `Cpl::System::Thread` objects that use 
periodic scheduling.  

Click [here](https://github.com/johnttaylor/colony.pico/blob/main/README.md) for additional details about the CPL threading on the PI Pico. 

Click [here](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/Io/Tcp/README.md) for additional details about the CPL non-blocking sockets 

#### Example Overview
The example project located under `projects/example/cpl_2threads/` directory is a simple blinky-LED application with a command shell that uses two threads.

The application blinks the PicoW board's LED at a runtime settable frequency and optionally outputs a status messages every time the LED state is toggled.  The LED application code executes on core0. The application also contains the Colony.Core's 'TShell' command processor (think Debug console) that runs over a TCP socket connection. The TShell command processor executes on core1.

The LED flash frequency and the terse/verbose state of the application is set by the developer entering TShell commands to set/change the LED timing and the terse/verbose state.

Below is example of the STDOUT output when the application starts-up and an external client makes a TCP connection (I am using [Putty](https://www.putty.org/))
```
**** APPLICATION START-UP *****
Cpl and the Bsp have been initialized.
Version: 7.95.49 (2271bb6 CY) CRC: b7a28ef3 Date: Mon 2021-11-29 22:50:27 PST Ucode Ver: 1043.2162 FWID 01-c51d9400
cyw43 loaded ok, mac 28:cd:c1:00:98:ab
API: 12.2
Data: RaspberryPi.PicoW
Compiler: 1.29.4
ClmImport: 1.47.1
Customization: v5 22/06/24
Creation: 2022-06-24 06:55:08
connect status: joining
connect status: no ip
connect status: link up
CONNECTED to the wife network: StarsHallow
My IP address: 192.168.1.38
>> 00 00:00:00.000 (app) [core0] Hello.  I am the Bob example application.
>> 00 00:00:00.000 (app) [APP-BOB] Enabling scheduling
>> 00 00:00:05.523 (app) [APP-BOB] Bob:delayModelPointChange()
>> 00 00:00:05.524 (app) [APP-BOB] Bob:verboseModelPointChange()
Accepted incoming connection. Remote Host: 192.168.1.72
```

Here is some example output from my Putty session.
```
help
bye [app [<exitcode>]]
dm ls [<filter>]
dm write {<mp-json>}
dm read <mpname>
dm touch <mpname>
help [* | <cmd>]
threads
tprint ["<text>"]
trace [on|off]
trace section (on|off) <sect1> [<sect2>]...
trace threadfilters [<threadname1> [<threadname2>]]...
trace level (none|brief|info|verbose|max)
trace here|revert
$ trace here
$ dm write {name:"delayTime",val:500}
$ >> 00 00:00:25.874 (app) [APP-BOB] Bob:delayModelPointChange()
dm write {name:"verbose", val:true}
$ >> 00 00:00:34.171 (app) [APP-BOB] Bob:verboseModelPointChange()
>> 00 00:00:34.301 (app) [APP-BOB] Bob: edge counter=99
>> 00 00:00:34.802 (app) [APP-BOB] Bob: edge counter=100
>> 00 00:00:35.301 (app) [APP-BOB] Bob: edge counter=101
>> 00 00:00:35.801 (app) [APP-BOB] Bob: edge counter=102
>> 00 00:00:36.301 (app) [APP-BOB] Bob: edge counter=103
>> 00 00:00:36.801 (app) [APP-BOB] Bob: edge counter=104
>> 00 00:00:37.301 (app) [APP-BOB] Bob: edge counter=105
>> 00 00:00:37.802 (app) [APP-BOB] Bob: edge counter=106
>> 00 00:00:38.301 (app) [APP-BOB] Bob: edge counter=107
>> 00 00:00:38.801 (app) [APP-BOB] Bob: edge counter=108
>> 00 00:00:39.301 (app) [APP-BOB] Bob: edge counter=109
>> 00 00:00:39.802 (app) [APP-BOB] Bob: edge counter=110
>> 00 00:00:40.301 (app) [APP-BOB] Bob: edge counter=111
>> 00 00:00:40.802 (app) [APP-BOB] Bob: edge counter=112
>> 00 00:00:41.302 (app) [APP-BOB] Bob: edge counter=113
...
```
 
__Notes:__
* The `help` command displays list of shell/console commands.  I use this to verify that connection is working.
* The `trace here` command routes the printf-tracing output to the command shell's output (instead of STDOUT)
* The `dm write {name:"delayTime",val:500}` command sets the LED delay time to 500ms, aka 1Hz
* The `dm write {name:"verbose", val:true}` and `dm write {name:"verbose", val:false}` controls the output messages from the application.
* The `bye` command exits the command shell and closes the socket connection.  The application will then accept a new connection and restart the command shell.
 

### Code Snippets
The example application was/is an existing application.  All I needed to was to
create a new `main()` function that does the board/platform specific setup and then call the application code.  __Almost__. Since servicing the TCP connection is done asynchronously I needed hooks into the application periodic scheduling for the TCP _background_ processing. This required refactoring the application to add three _hook_ functions that are called as part of the periodic processing for the two threads/core. Here is code for [main()](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/wifi/picow/main.cpp) and the [run_application()](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/application.cpp) method it calls.  The `core0MBox_` and `core1Mbox_` objects are  `Cpl::System::Runnable` instances (think the main/forever loop for each thread).

```
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
```
```
/// Application Code
void runApplication()
{
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am the Bob exampleapplication.") );

    // Create mock application thread 
    Cpl::System::Thread::create( core0Mbox_, "APP-BOB" );

    // Create a 'Scheduler' thread for the TShell to run in
    Cpl::System::Thread::create( core1Mbox_, "TSHELL" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
}
```