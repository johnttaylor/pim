# colony.core
A C++ Class library targeted specifically for embedded development.

Colony.Core is a redesign/refactor of the Jcl C++ Class library from the Shift-Right Open Repository (www.integerfox.com/openrepo). Some of the changes include:
  - Bug fixes
  - Cleaner and/or more consistent interfaces
  - Unittests for all namespaces/packages.
  - All interfaces/namespaces support simulate time.
  - Works (Compiles, link, runs, etc.) on 64bit platforms as well as with C++11 compilers.

Colony.Core is also part of a proof-of-concept for the Outcast project (www.integerfox.com/outcast). Outcast is 
a project that attempts to manage code reuse in a scalable, team oriented environment. Colony.Core is a C++ 
class library that is designed from ground up to support software reuse. To achieve extensive software reuse, 
attention has to be paid to managing dependencies between elements. To this end, the following features where 
implemented to achieve the reuse goal:

  - Source code files organized independent of the projects that use them.
  - Directory layout not based on the traditional "top-down call tree" paradigm, but structured to reflex the actual 
    dependences of the code modules.
  - A reusable and extendable build environment.
  - Extensive use of Object Orient Design methodologies to make the source code modules as loosely coupled as possible.

Another characteristic of the Colony Packages are that the majority of the code was developed for embedded software 
development. And as such, was designed to meet the constrained environment of embedded programming. The most obvious
characteristics are the obsession with deterministic memory management and strict type checking. The following traits 
are typical of embedded systems.

  - Memory constrained. Embedded systems have limited ROM, FLASH, and RAM memories.
  - Dedicated system. Embedded hardware typically runs one and only one application.
  - High availability. Most embedded systems are expected to run once power is applied and continuing running until 
    power is lost/turned off. This means a embedded application is expected to run for months and/or years without 
    rebooting.
  - Real time. Embedded systems interact with the physical world. This interaction involves hardware inputs and 
    controls. The software must respond to this stimuli in a deterministic and timely fashion.
  - Stand alone. There is no standard hardware platform or RTOS for embedded applications. Typically, the hardware 
    supporting the application is very specialized and proprietary. Unique hardware translates to unique software 
    (i.e. limited COTS software support).
  - Platform changes. Designers of embedded systems have to deal with the possibility of the underlying hardware 
    and/or RTOS being changed. Also hardware platforms are constantly being upgraded or cost reduced. If the hardware 
    changes, the software must change with it.


Documentation is located in the top/ directory and/or availabe at: www.integerfox.com/colony.core.

To build/use Colony.Core within the Outcast model - see the installation instrucation at: www.integerfox.com/outcast/start_here.html#Installing

Colony.Core is licensed with a BSD licensing agreement (see the top/ directory).

