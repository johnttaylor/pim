# The Main Pattern: A Case Study
In my book - [Patterns in the Machine: A Software Engineering Guide to Embedded Development](https://www.amazon.com/Patterns-Machine-Software-Engineering-Development-dp-1484264398/dp/1484264398/ref=mt_other?_encoding=UTF8&me=&qid=) - I discuss the `Main Pattern`.  The *Main Pattern* states that an application is built by wiring together independent components and modules. The Main pattern consists of:

  - The resolution of interface references with concrete implementations.
  - The initialization and shutdown sequencing.
  - The optional sequencing (or runtime execution) of a set of components and modules. The “set of components and modules” can be the entire application, a sub-system, a feature, or any combination thereof.

The book also provides an [example Thermostat application](https://github.com/johnttaylor/pim) that was built using various best practices from the book including the *Main Pattern*.  To demonstrate and to some extent validate how effective the *Main Pattern* is I ported the example application to a Raspberry Pi Pico board with a small graphic LCD.  The Pico projects are located under `projects/Storm/Thermostat/Pico/` directory.  The rest of this README page describes the challenges, process, and the final outcome of the exercise. 

Additional details on how to run the Pico variant of the Thermostat can be found [here](https://github.com/johnttaylor/colony.pico/blob/main/projects/Storm/Thermostat/Pico/README-PicoStormThermostat.md)

#### TL;DR
The *Patterns in the Machine* book takes the [Open/Close](https://en.wikipedia.org/wiki/Open%E2%80%93closed_principle) principle quit literally in that:
  > Adding new functionality should not be done by editing existing source code.
   
 In a perfect world, the Pico port should have be able to be done without changing any of the original source code.  In that respect, the original application design *failed.*  The port required me to modify 7 existing files:
  - Two build script files (to move a common source directory entry to be platform specific).
  - Two Algorithm source files so that the Algorithm does not require a dedicate thread.
  - Two top level *Main Pattern* files to break out some non-platform specific initialization so as not to replicated start-up logic in the Pico port.
  - The simulation's `platform.cpp` file had to be split up into two parts - one for the platform specific actions and one for object creation to allow for different behavior on the Pico simulation without replicating original simulation's object creation logic.

So not a an **A+**, but I claim it is definitely a pass grade :). If this was a real world scenario of having to port my product to new hardware and I only had to make the types of changes listed above - I would be hero.  I will go further and claim that you can build production embedded software that is change tolerant and extensible in the same amount of time (or less) than a monolithic approach that is prevalent today in the embedded space.


# Requirements
I gave myself the following requirements for porting the Thermostat example code to the Pico.
  - Execute the example code on the Pico hardware platform.
  - Add a partial, primitive GUI to the application.
  - Include a functional simulator of the Pico port with its GUI that executes on a PC
     
# Constraints
The example Thermostat application is a incomplete application in the senses that it has no actual temperature inputs, relay outputs, or User Interface.  It has internal interfaces for all of the above - I just didn't have the time when writing the book to flush out a complete product.  That said the application does contain the following:
  - A PID based Temperature Control algorithm that supports Heat/Cool systems with zero to a single stage air conditioners and up to 3 stages of indoor heat.
  - Since there are no physical inputs/outputs, a *house simulator* thats provide a crude closed loop simulation of a house and the effects of Outdoor temperature, Indoor temperature, Setpoints, and the output capacity of the HVAC system. 
  - A Debug Console that provides commands to fully exercise and monitor the thermostat's control algorithm.
  - Persistent storage for the Thermostat's Installer and Home owner settings.
  - Interfaces (via the Data Model) for integrating temperature inputs and relay outputs.
  - Interfaces (once again via the Data Model) for constructing a UI.

## Pico platfrom
My Pico hardware consists of a Raspberry Pi [Pico board](https://www.raspberrypi.com/products/raspberry-pi-pico/) (aka a RP2040 dual core MCU) with a Pimoroni [Pico-Display](https://shop.pimoroni.com/products/pico-display-pack?variant=32368664215635) (1.14" graphic LCD with 4 momentary push button and a RGB LED).  

Because the Thermostat application uses/is-dependent on the [Colony.Core CPL C++ Class library](https://github.com/johnttaylor/colony.core/blob/master/README.md) - the Pico port will also use the same class library.  However, the C++ class library's currrent [threading support for Pico](https://github.com/johnttaylor/colony.pico/blob/main/README.md) only provides up to 2 thread - one for each core.

### Differences
The following are features, sub-systems, architecture, etc. that do not port *well* (i.e. without some kind of change or extension) to the Pico.
  - The number of threads.  The Thermostat application running on its original target hardware consist of the 4 Threads listed below.  The Pico only has two threads.
    - Control Algorithm
    - Persistent Storage record server
    - Debug Shell
    - House Simulator
  - Due to the thread limitations on the Pico - the Pico port requires the use of using Periodic Scheduler runnable objects (vs event based Mailboxes).  This has an impact on how in thread initialization and shutdown is done.  I will explain later in more detail about difference betweens a Periodic Scheduler and a Mailbox.
  - Persistent Storage.  The original target hardware has external data flash for persist storage.  My Pico hardware (currently) does not have any application data persistent storage.
   


# Solutions
Below is how above issues were resolved.

### Persistent Storage
The short answer is the Pico port does not support persistent storage.  However, the construction and start-up of the application includes all of the necessary *plumbing* except that a *null* media driver was used in lieu of media driver that performs the actual reads/writes to persistent storage.  This means that once some form of physical persistent storage is added by my Pico platform the only change is replace the *null* media driver with the *real* media driver.

  **Note:** The simulator for the Pico port does support persistent storage.

### Periodic Scheduler vs Mailbox Server
A Periodic Scheduler runnable object is a child class of a MailBox Server.  The usage difference between the two types of runnable objects are that the Periodic Scheduler has *begin thread execution* and *end thread execution* callbacks that the application uses to perform in thread initialization/shutdown of objects that execute in the Periodic Scheduler's thread. When using a Mailbox Server, the in thread initialization and shutdown is done using Synchronous ITC messaging to invoke open/close methods on the objects that execute in the Mailbox Server's thread.  However, this approach effectively requires a dedicated startup/shutdown thread that manages all of the other threads.  The two thread limit on the Pico platform does not make this approach feasible. 

The solution was to use a Periodic Scheduler for the Core0 thread, and Mailbox Server for the Core1 thread.  Then in the *begin/end thread execution* callback methods for Core0, the appropriate Synchronous ITC open/close requests are made to objects that execute in the Core1 thread.  Unfortunately this approach made reusing the code in the `src/Storm/Thermostat/Main/Main.cpp` file for application creation and sequencing not possible.  

A new application creation/sequencing file (`src/Storm/Thermostat/Main/_pico/Main.cpp`) was created for the Pico port. A remaining problem was that the original `Main.cpp` file provided the model point initialization values.  The same initialization code would be needed for the Pico port. In order to not replicate the model point initialization method - the decision was made to modify the original `Main.cpp` file to move the model point initialization into a separate file (`modelpoints.cpp`).  This also required the `Private_.h` header file be updated to include the function prototype for the model point initialization function.  

One final change had to be made and that was to the build script `projects/Storm/Thermostat/libdirs.b` file.  This file contains all of the source code directories that are common between **all** the Thermostat application variants.  The change was to move the existing entry for the `src/Storm/Thermostat/Main/` directory into a variant specific `libdirs.b` file.

### Threads
Here is how the final sub-system/threads came out for the Pico Port
- Core0 thread: Runnable Object is Periodic Scheduler
    - UI sub-system
    - Debug Console
    - House Simulation
- Core1 thread: Runnable Object is Mailbox Server
  - Thermostat Algorithm
  - Persistent Record Server

#### UI 
Since this is new sub-system to the application the only constraint is that it needs to execute in a *responsive* thread.

Slightly off topic - the entire UI was developed using the [functional simulator](https://github.com/johnttaylor/colony.pico/blob/main/projects/pico-sketch/README.md#GettingStarted).  No debugging, refactoring, etc. was need for running the UI on the physical hardware.

#### Debug Console
The CPL C++ Class library provides support for executing the Debug console in a dedicate thread with blocking read semantics (i.e. the original Thermostat application approach) **or** sharing a thread using a Periodic Scheduler runnable object using polling read semantics.

#### House Simulation
The House Simulation class is designed as runnable object to run exclusively in own thread.  However, this design was really a quick-n-dirty solution to provide periodic scheduling to the simulation.  The decision was made to create a sub-class (`ScheduledSimHouse.h`) of the House Simulation object to expose its execution method and then call this public method directly from the Periodic Scheduler running on Core0.  No changes to the existing House Simulation source code were needed.

#### Thermostat Algorithm
The original design of the Algorithm object was as a runnable object that executed in a dedicated thread.  With the 2 thread limit of the Pico - this was not acceptable.  However, thinking about the original design: it was silly and/or lazy because the Algorithm code executes once every 2 seconds - hardly a persuasive argument for requiring a dedicated thread.  The Algorithm class was changed from inheriting from the Runnable object class, to having a Runnable object provided in its constructor.  

This change also required the Algorithm's unit test project to also be updated.  That said, it was very comforting to make the two line change to the unit test project, build, and then run the unit test and have it pass.

#### Persistent Record Server
As stated earlier the Pico port doesn't really support persistent storage.  However it is built *persistent storage ready*.  The issue with the Persistent Record Server is that it too is runnable object that uses a dedicate thread.  This design is because the write/erase/read operations to/from the persistent storage media are done as synchronous blocking/busy-wait operations.  

This above approach was not changed or extended for the Pico Port to accommodate executing the Algorithm in the same thread as the Persistent Record Server.  Instead a rationalization was made that claims that any blocking/busy-wait media operation related to persistent storage would be sufficiently short/bounded with respect the Algorithm execution interval of once every 2 seconds.  Also, given the nature of how infrequently persistent storage is updated after start-up - any timing disruptions (if at all) would be short-lived in nature.

#### Functional Simulator
The PC simulation of the Pico port is slightly different from the original simulation because the Pico port has a UI and how the Pico port using a Periodic Scheduler.  The differences between the two simulation variants manifested in the body of the *platform* interface functions (see `Private_.h`) used by the Application's *Main Pattern*.  However, all of the objects created by the original simulation were needed/used by the Pico's simulation.  The choice was made to separate out the *platform* methods from the object create in the simulation's `platform.cpp` file (the object creation was put into a new `statics_.h` file).  This allowed a single source for the common objects that the simulation variants require.

# Summary
While this was just *academic* exercise, it contains sufficient details and nuisances to be representative of real world design challenges and best practices.  Yes I had to modify 7 existing files from the original application - but it was always wishful thinking that no files would have to be modified.  I would argue that really on the two files `src/Storm/Thermostat/Algorithm.h|cpp` *broke* the goal of the [Open/Close](https://en.wikipedia.org/wiki/Open%E2%80%93closed_principle) principle since the other 5 files belong to parts of the system that are highly dependent on most everything and not worth the investment to make them closed to modification and 100% open to extension.  And to be honest the original design of `Algorithm` class was a poor choice on my part and if it had been done better, then only 5 files would have had to be changed and no OCP violations ;-).

**Post Script:**  Once the GUI was up and running a somewhat *embarrassing* bug was found in the Thermostat Algorithm.  The bug was the *Fan Continuous* operation did not work when the thermostat was in the `OFF` mode (it has been fixed in this repository).  This just proves rule #1 from chapter 17 my book ;-) 
  >Never trust the software guys
 
