/** @namespace Cpl::Io::Tcp::lwIP::Picow

The 'Picow' namespace implements the TCP interfaces for the Picow platform. The
dependencies for the implement are:

- The Pico C/C++ SDK.  
- Built with the pico_cyw43_arch set to polling or thread_safe_background
    CYW43_LWIP=1
    PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1 OR PICO_CYW43_ARCH_POLL=1
    
    Note: When building with PICO_CYW43_ARCH_POLL=1, all Wifi/TCP operation
          must be performed on the SAME core because of the SDK interrupt
          handling is not protected against multiple cores. Build with
          PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1 if you need to have Wifi/TCP
          operations execute on both cores.

- Built with the lwIP config:
    NO_SYS=1    

*/  


  