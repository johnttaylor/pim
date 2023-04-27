/** @namespace Driver

The 'Driver' namespace provides a collection of platform/target independent 
concrete device drivers.  Note: Some of the drivers maybe very specific to a
hardware device, peripheral, IC, etc.  However all of the drivers will have a
defined Hardware Abstraction Layer (HAL) - that allows the concrete driver to be
decoupled from a specific target/BSP.  This paradigm allows reuse of the drivers
when the some device/peripheral is common across multiple physical targets.

The various driver interfaces are/were created as needed, i.e. there is NO grand
plan to design the canonical set of drivers.  In addition, the are/will-be
drivers that are very similiar - but different for some reason or other.  This
is OKAY.  If an existing driver make sense for you application - then use it.
What you should NOT do - is for force your application design to accommodate
an existing driver interface (i.e. instead create a new driver that meets your 
requirements).

*/  


  