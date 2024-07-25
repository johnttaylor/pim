/** @namespace Cpl::Io::Serial::ST::M32F4

The M32F4 namespace provides implementation of the Cpl::Io::Output interface
over a serial port using a STM32F4xxx MCU. 

NOTE: There appears to be a macro defined as STM32F4 in the HAL stuff -->e.g. 
      cant use STM32F4 as namespace name

The implementation is DEPENDANT on FreeRTOS and ST's HAL layer/SDK for managing 
the UART registers, state, interrupts, etc.

*/  
