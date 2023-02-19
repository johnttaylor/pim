/** @namespace Cpl::System::RP2040

Concrete implementation of the platform specific classes/features of the 
Cpl::System namespace interfaces for the Raspberry RP2040 MCU's dual cores,
i.e. a maximum of 2 threads - one per core.

Platform Dependent Behaviors:

Thread      - The first thread created runs on core0.  A SINGLE additional 
              thread can be created that runs on core1.

Mutex       - Fully functional.  Note: The memory for internal Mutex type 
              is allocated from the Heap.

GlobalLock  - The GlobalLock maps the RP2040 SDK's critical section.  There are
              some subtle details here - but it effective disables IRQs on 
              the calling core AND provides mutual exclusion with respect to
              the other core.

Semaphore   - Fully functional.  Note: The memory for internal Semaphore type 
              is allocated from the Heap.

Tls         - Fully functional.

ElapsedTime - Fully functional.  
              
EventLoop   - Fully functional.  This includes EventFlags and Software Timers.


Application Start-up sequence:
==============================
1. MCU Resets.

2. Core0 begins executing. Core1 is in the sleep state.

3. The bootloader(s) execute and transfer execution to the Application.

4. The Application's C/C++ start-up code executes (e.g. crt0, low-level SDK initialization, etc. ).

5. The Application's C/C++ main() method is called.  
    - At this point the system is a bare metal system with interrupts disabled.
    - The application code should call Cpl::System::Api::initialize() ASAP.
    - The application should complete any required HW/BSP/App initialization 
      that needs to occur before 'thread scheduling' begins.
    - The Application needs to call Cpl::System::Thread::Create() to create 
      at least one 'thread'.  The application can create at most two 'threads'.
      The first thread created will execute on CORE0, the second thread executes
      on CORE1

6. The Application 'starts threading' by calling Cpl::System::Thread::enableScheduling().

7. Once the 'threading' has been enable, the Application can optional create the second
   thread assuming only 1 thread was created prior to enableScheduling() call. 

Notes: 
    1. The second thread can be forcibly stopped (i.e. CORE1 put into the sleep state)
       by calling Cpl::System::Thread::destroy().  If the second thread terminated itself,
       i.e. the Runnable object ran to completion, CORE1 will be put into the sleep state.
    2. A second thread can be created 'again' after destory() has been called.
    3. Calling destroy() on the first thread has NO effect.
*/  


  