/** @namespace Driver

The 'Driver' namespace provides a collection of platform/target independent 
concrete device drivers.  Note: Some of the drivers maybe very specific to a
hardware device, peripheral, IC, etc.  However all of the drivers will have a
defined Hardware Abstraction Layer (HAL) - that allows the concrete driver to be
decoupled from a specific target/BSP.  This paradigm allows reuse of the drivers
when the some device/peripheral is common across multiple physical targets.

*/  


  