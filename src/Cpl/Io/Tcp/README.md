# Non-blocking TCP Sockets

This namespace defines interfaces for creating non-blocking `Cpl::Io` streams using 
TCP/IP connections. This provides an alternative non-blocking approach vs. 
traditional BSD Sockets.  The interfaces are designed to be compatible with the
`Cpl::System::PeriodicScheduler` and/or _runnable_ objects/threads that 
support periodic scheduling.  

The are three primary interfaces:
* [Listener](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/Io/Tcp/AsyncListener.h)
* [Connector](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/Io/Tcp/AsyncConnector.h)
* [InputOutput](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/Io/Tcp/InputOutput.h) (aka the IO Stream)

The motivation for the non-blocking TCP support comes from wanting to use the [Raspberry PI PicoW's](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
native WIFI without an RTOS.  However, the interfaces are __not__ specific to the 
PicoW (e.g. there is a Win32 implementation).


### PicoW Implementation
The PicoW implementation is built on top of the Pico C/C++ SDK and its integration
with the [lwIP](https://savannah.nongnu.org/projects/lwip/) networking stack. In
addition, the implementation is thread[^1]/multi-core safe. The required build 
options/switches are:

* `LIB_PICO_CYW43_ARCH=1`
* `CYW43_LWIP=1` 
* `PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1` or `PICO_CYW43_ARCH_POLL=1`

__Note:__ The above settings sets the lwIP configuration to `NO_SYS=1`

The implementation is located in the `src/Cpl/Io/Tcp/lwIP/Picow` directory.

[^1]: In this context _threads_ are the `Cpl::System::Thread` instances and when built with PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1.  


### Win32 Implementation
The Win32 implementation use the Winsock DLL to implement the interface.
The implementation uses WSA sockets configured for non-blocking
operation.

The implementation is located in the `src/Cpl/Io/Tcp/Win32` directory.


### Test Applications
There are two test applications used to verify the implementations. One of
the applications is a TCP server/listener that accepting incoming connection
requests.  The second one is client/connector that connects to a remote
host.  The test code is located in the `src/Cpl/Io/Tcp/_0test` directory.  The
test applications are built under the `tests/Cpl/Io/Tcp/_0test/` directory tree.

The applications are meant to be used as a pair.  The server application echoes 
back the input it receives from the client application.  The client application 
verifies that the data it sends is the data it receives.  __Note__: The server
application can be used as a stand alone TCP echo server.

#### Testing Examples 
1. To test the PicoW `AsyncListener` implementation
   * Build the `tests/Cpl/Io/Tcp/_0test/server/lwip/picow-uart/windows/gcc` test project
     * Flash the `tcp-server` image onto the PicoW
   * Build the `tests/Cpl/Io/Tcp/_test/client/win32/vc12` test project.
     * Run the `_win32\bb.exe` to connect to the PicoW test client application
2. To test the PicoW `AsyncConnector` implementation
   * Build the `tests/Cpl/Io/Tcp/_test/server/win32/vc12` test project.
     * Run the `_win32\bb.exe` to begin listening for a connection.
   * Build the `tests/Cpl/Io/Tcp/_0test/client/lwip/picow-uart/windows/gcc` test project
     * Flash the `tcp-client` image onto the PicoW

__Notes__: 
1. When building the PicoW projects the following configuration is set via 
          the build scripts (i.e. edit the `mytoolchain.py` file in the build directory)
  * `WIFI_SSID=`\<my network's SSID\>
  * `WIFI_PASSWORD=`\<my wifi password\>
2. When building a client/connector application the IP address of the remote host is
   specified in the `colony_config.h` file (located in the build directory)
   * `#define OPTION_REMOTE_HOST_ADDRESS "192.168.1.72"`
    
  