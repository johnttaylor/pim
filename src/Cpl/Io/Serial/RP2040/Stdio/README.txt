/** @namespace Cpl::Io::Serial::RP2040::Stdio

The Stdio namespace implements the Cpl::Io streams utilizing the Raspberry PI 
Pico C/C++ SDK's stdio interfaces.  The SDK's stdio interface supports using a 
physical UART on the MCU, or a virtual UART over USB (i.e. as a CDC USB device).
Which physical media is used is a compile/build time decision.

NOTE: The Pico C/C++ SDK does NOT DEFINE any semantics with respect to buffering
      of input/output data over stdio.  For example, when using the UART for
      stdin - there is no background ISR that is caching incoming data bytes
      until a read() method is called.  However, the USB implementation of stdin
      does do some internal caching (how much caching is unclear).  
      
      BUYER BEWARE (or use the Cpl::Io::Serial::RP2040::Uart implementation 
      instead).

Note: The implement does NOT support the blocking semantics of the Cpl::Io
      streams.  The design decision was for the implementation to be compatible/
      useful on bare-metal systems, i.e. no threads required.

*/  


  