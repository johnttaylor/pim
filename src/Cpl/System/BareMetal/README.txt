/** @namespace Cpl::System::BareMetal

Concrete implementation of the platform specific classes/features of the 
Cpl::System namespace interfaces for a Baremetal/Singled-threaded system.

Platform Dependent Behaviors:

Thread      - The threading class is stubbed out so that any call to the 
              interface are benign.  The Thread interface will "report" a 
              single thread.  The application cannot create threads.

Mutex       - The mutex interface is effectively all NOPs

GlobalLock  - The GlobalLock can be mapped to a platform's EI/DI calls, or
              to NOPs (on the Windoze, this mapped to NOPs)

Semaphore   - Fully functional.  However since there is only 'one thread' - if 
              a semaphore's count is at zero and the wait() method is called the
              "main thread" will block/busy-wait forever unless the semaphore's
              su_signal() method is called from an ISR context.

Tls         - Fully functional.

ElapsedTime - The platform is required to provide the number of milliseconds
              since power-up.  
              
EventLoop   - Fully functional.  This includes EventFlags and Software Timers.

*/  


  