/** @namespace Cpl::Io::Serial::RP2040::Uart

The Uart namespace implements the Cpl::Io streams utilizing the Raspberry PI 
Pico C/C++ SDK's UART interfaces.  The implement provide a more robust stream 
interface than using Cpl::Io::Serial::RP2040::Stdio because it is interrupted 
driven and provides software TX/RX FIFOs.

Note: The implement does NOT support the blocking semantics of the Cpl::Io
      streams.  The design decision was for the implementation to be compatible/
      useful on bare-metal systems, i.e. not threads required.

*/  


  