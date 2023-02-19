# Porting the Cpl OSAL to a new Target
The Cpl C++ class library provides a OS Abstraction Layer (OSAL) that decouples 
the rest of the class library from target platforms.  This page is an 
introduction to porting the OSAL layer to a new target platform.

__TL;DR__ for those who want to jump right in - you can clone one of the existing 
ports - [Win32](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Win32), 
[Posix](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Posix), 
[C++11 concurrency](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Cpp11), 
[FreeRTOS](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FreeRTOS), 
[Raspberry PI Pico](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/RP2040), 
[Bare Metal](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/BareMetal) - 
and edit as appropriate for your platform.  

As the name implies, the Cpl OSAL is meant to provide an abstraction for and/or 
decoupling from the underlying operating system.  However, as demonstrated by the
[Raspberry PI Pico](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/RP2040) and 
[Bare Metal](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/BareMetal)
implementations - an underlying Operating System is not a requirement.  In addition, the OSAL
can be ported to run on non-RTOS environments such as Windows and Linux.

There are 14 different interfaces that make up the OSAL.  This may sound daunting,
but its straight-forward to port the individual interfaces.  The exception
to this is the _Threading_ interface - which is the most involved when it comes
the target implementation.

* [Assert](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Assert.h)
* [Elapsed Time](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/ElapsedTime.h)
* [Fatal Error](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FatalError.h)
* [File and Directory management](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Io/File/Api.h)
* [Global Lock](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/GlobalLock.h)
* [Mutex](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Mutex.h)
* [Newline](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Io/NewLine.h)
* [Semaphore](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Semaphore.h)
* [Shell](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Shell.h)
* [Shutdown](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Shutdown.h)
* [System Api](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Api.h)
* [Threads](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Thread.h)
* [Thread Local Storage](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Tls.h)
* [Tracing](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Trace.h)

The only other caveat is that the developer who is doing the porting needs to be 
__experienced/knowledgeable__ about the target platform.

## Decoupling techniques
For the most part, the decoupling of the OSAL from the target platform is done
using [link time binding](https://patternsinthemachine.net/2023/02/breaking-dependencies-using-binding-times/), 
and the [LHeader pattern](https://patternsinthemachine.net/2023/02/lheader-and-lconfig-patterns/). 
I encourage you to review the previous links before continuing.

Basically, each target platform has it owns implementation of
the above interfaces.  As part of the LHeader pattern - there is a `mappings_.h`
header file that provides the target specific mappings and/or data types.

__Caution__: If your application contains C code that is developed in-house (i.e. 
__not__ third-party code), you should avoid putting C++ specific types/constructs 
in the `mappings_.h` header file.  This allows your C code to call non-C++ 
specific interfaces such as the Cpl `Assert` interface.  See the FreeRTOS [`mappings_.h`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FreeRTOS/mappings_.h), 
 [`c_assert.h`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/_assert/c_assert.h),
and [`c_assert.cpp`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/_assert/c_assert.cpp)
files as examples.


## Runtime initialization
The Cpl framework provides a [mechanism](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/PrivateStartup_.h) 
that allows modules to register for callbacks when the framework is initialized (i.e. when `Cpl::System::Api::initialize()` is called).  

In the context of porting the OSAL, this start-up feature can be used provide runtime 
initialization for statically allocated instances.  For example on the 
[Raspberry PI Pico](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/RP2040)
target, the SDK's mechanism for statically creating mutexes and semaphores does
not 'play well' with a C++ object design. This in turn leads to the OSAL implementation 
having to make explicit calls - after `main()` is invoked - in order to initialize
mutex and semaphore structures. However, the OSAL semantics allow for mutexes 
and semaphores instances to be created/allocated statically (i.e. their constructors 
execute _before_ `main()` is called).  For the PI Pico implementation these classes 
use the Cpl start-up feature to initialize their underlying SDK 
[mutex](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/RP2040/mutex_t_.h)/[semaphore](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/RP2040/semaphore_t_.h) 
structures after `main()` is called.  

_Note_: The Cpl framework also provides [mechanism](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Shutdown.h) that allows modules to register for callbacks when the 
framework is being shutdown.

## Interfaces
__Note__: The provided code snippets are simplified pseudo code of the actual 
interfaces.  This is done to provide some context for the reader without having
to following the hyper-links.  The headers files are the _source of truth_ for
the OSAL interfaces (i.e. this page maybe/will-become stale over time).
 
### Assert
The [Assert](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Assert.h) interface
only requires a platform specific implementation of the `CPL_SYSTEM_ASSERT()` macro.
Typically, this macro is mapped to the Cpl `FatalError` interface. For example:

```
file: mappings_.h
------------------
#define CPL_SYSTEM_ASSERT_MAP(e)  do { if ( !(e) ) Cpl::System::FatalError::logf( "ASSERT Failed at: file=%s, line=%d, func=%s\n", __FILE__, __LINE__, CPL_SYSTEM_ASSERT_PRETTY_FUNCNAME ); } while(0)
```
One case where this should __not__ be done is if the your application contains
C files and it is desirable to be able to call the `CPL_SYSTEM_ASSERT` macro
from within these files. For this case - the assert macro must map to _something_ 
that is legal C code.  See the FreeRTOS [`mappings_.h`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FreeRTOS/mappings_.h), 
 [`c_assert.h`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/_assert/c_assert.h),
and [`c_assert.cpp`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/_assert/c_assert.cpp)
files as examples. 

### Elapsed Time
The [Elapsed Time](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/ElapsedTime.h) 
interface has the following methods that require a platform specific implementation:

```
unsigned long millisecondsInRealTime( void ) noexcept;
unsigned long secondsInRealTime( void ) noexcept;
Precision_T   precisionInRealTime( void ) noexcept;

unsigned long milliseconds( void ) noexcept;
unsigned long seconds( void ) noexcept;
Precision_T   precision( void ) noexcept;
```

__However__, there is a [default implementation](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/ElapsedTime2.cpp) of the `xxxxInRealTime()` methods
so that a new target only needs to supply a basic [get-elapsed-time-in-milliseconds](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/PrivateElapsedTime_.h)
function and then have the `milliseconds()`, `seconds()`, and `precision()` methods
call the corresponding `xxxxInRealTime` methods.  

If you do not what to use
the default implementation for the `xxxxInRealTime()` methods - provide your
own implementation and do __not__ build the `ElapsedTime2.cpp` file in the
`src/Cpl/System/` directory.

__Why__ are there two methods - `milliseconds()` and `milliseconsInRealTime()` - for 
getting elapsed time in milliseconds?  This is because the Cpl framework and the OSAL support
[simulated time](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/SimTick.h).
If your target platform is a non-process based RTOS - you typically don't have a use case
for supporting simulated time - so simply map `milliseconds()`, `seconds()`, 
and `precision()` methods to call the corresponding `xxxxInRealTime()` methods. 

If you want/need to support simulated time on your target platform - use the 
[Win32](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Win32) and 
[Posix](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Posix)
implementations a guide/template.


### Fatal Error
The [Fatal Error](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FatalError.h) 
interface has the following methods that require a platform specific implementation:
```
    void log( const char* message );
    void log( const char* message, size_t value );
    void logf( const char* format, ... );

    void logRaw( const char* message );
    void logRaw( const char* message, size_t value );
```
It is fairly simply to implement these method for a new target.  In addition, 
the target implementation has the freedom to interpret/ignore the semantics
of the log messaging.  What matters is that: 1) the functions are provided, 
and 2) they are implemented in such way that you can set breakpoints in the 
function bodies when running on the target.

Another nuance to the _Fatal Error_ interface is that while it is target specific,
it can _also_ be project specific.  For example I can be using FreeRTOS on
boardA/ProjectA and boardB/ProjectB - but because of the differences in the 
boards/MCUs between the Projects - handling of the fatal errors 
needs to be done differently.  What this means for the porting exercise, is you
should organize your code (and build scripts) such that each project can select 
or provide its own implementation of the _Fatal Error_ interface.



### File and Directory Management
The [File and Directory management](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Io/File/Api.h)
interface only requires a platform specific implementation for the `CPL_IO_FILE_NATIVE_DIR_SEP` 
and `CPL_IO_FILE_MAX_NAME` symbols.  For example:

```
file: mappings_.h
------------------
/// Win32 Mapping
#define CPL_IO_NEW_LINE_NATIVE_MAP      "\015\012"

/// Win32 Mapping
#define CPL_IO_FILE_NATIVE_DIR_SEP_MAP  '\\'
```

This interface can be skipped completely if you don't plan to using the `Cpl::Io::File`
interfaces.  As an alternative, you can require individual projects to provide the 
mapping (based on their usage of the `Cpl::Io::Filo` interfaces). 


### Global Lock
The [Global Lock](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/GlobalLock.h) 
interface has the following methods that require a platform specific implementation:
```
void begin( void );
void end( void );
```

These methods are essential wrappers for enabled/disable interrupts.  For targets
(e.g. Windows, Linux, etc.) where the OSAL port is intended to run in _user space_
(i.e. non-privilege mode) this interface can be mapped/implemented using a mutex.

### Mutex
The [Mutex](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Mutex.h)
interface consist of the following class.  An instance of this class is created
per mutex instance.
```
class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock( void );
    void unlock( void );

protected:
    Cpl_System_Mutex_T  m_mutex;
};
```

The definition of `Cpl_System_Mutex_T` is provided in the `mappings_.h` header file. For
example:
```
#define Cpl_System_Mutex_T_MAP   pthread_mutex_t
```

__Note__: The OSAL mutex semantics are for a recursive mutex


### Newline
The [Newline](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Io/NewLine.h) interface
only requires a platform specific implementation of the `CPL_IO_NEW_LINE_NATIVE` 
symbol.  

```
file: mappings_.h
------------------
#define CPL_IO_NEW_LINE_NATIVE_MAP   "\n"

```

This interface can be skipped completely if you __never__ plan to use any of 
the `Cpl::Io` interfaces.  As an alternative, you can require individual projects to 
provide the mapping (based on their usage of the `Cpl::Io` interfaces).  

__Caution__: Rule #1 - _Never believe Never_

### Semaphore
The [Semaphore](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Semaphore.h)
interface consist of the following class.  An instance of this class is created
per semaphore instance.
```
class Semaphore : public Signable
{
public:
    Semaphore( unsigned initialCount=0 );
    ~Semaphore();

    void wait( void ) noexcept;
    bool timedWait( unsigned long timeout ) noexcept;
    bool tryWait( void ) noexcept;

    int signal( void ) noexcept;
    int su_signal( void ) noexcept;

protected:
    Cpl_System_Sema_T  m_sema;

    void waitInRealTime( void ) noexcept;
    bool timedWaitInRealTime( unsigned long timeout ) noexcept;
};
```

The definition of `Cpl_System_Sema_T` is provided in the `mappings_.h` header file. For
example:
```
#define Cpl_System_Sema_T_MAP   sem_t
```

Similiar to the `ElapsedTime` interface, the Semaphore interface has a _time_ 
aspect that is part of the Cpl framework's [simulated time](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/SimTick.h)
feature.  If your target platform is a non-process based RTOS - you typically 
don't have a use case for supporting simulated time - and the implementation 
for the `wait()` and `waitInRealTime()` methods are identical, as is the 
implementation of the `timedWait()` and  `timedWaitInRealTime()` methods. 

If you want/need to support simulated time on your target platform - use the 
[Win32](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Win32) and 
[Posix](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Posix)
implementations a guide/template.


### Shell
The [Shell](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Shell.h)
interface has the following methods that require a platform specific implementation:
```
   int execute( const char* cmdstring, bool noEchoStdOut = true, bool noEchoStdErr = true );
   bool isAvailable();
```

And platform specific implementation of the `CPL_SYSTEM_SHELL_NULL_DEVICE_` and 
`CPL_SYSTEM_SHELL_SUPPORTED_` symbols.  

```
file: mappings_.h
------------------
/// Win32 Mapping
#define CPL_SYSTEM_SHELL_NULL_DEVICE_x_MAP  "NUL"

/// Win32 Mapping
#define CPL_SYSTEM_SHELL_SUPPORTED_x_MAP    1
```

Even if your projects do not use/require this interface - it is recommended to
provided a _empty_/NOP implementation of this interface.

### Shutdown
The [Shutdown](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Shutdown.h)
interface has the following methods that require a platform specific implementation:
```
int success( void );
int failure( int exit_code );
```
Similiar to the `FatalError` interface, the shutdown logic can 
_also_ be project specific.  What this means for the porting exercise, is you
should organize your code (and build scripts) such that each project can select 
or provide its own implementation of the _Shutdown_ interface.

### System API
The [System Api](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Api.h)
interface has the following methods that require a platform specific implementation:
```
void initialize( void );
void enableScheduling( void );
bool isSchedulingEnabled( void );

void sleep( unsigned long milliseconds ) noexcept;
void sleepInRealTime( unsigned long milliseconds ) noexcept;

void suspendScheduling(void);
void resumeScheduling(void);
```

The implementation of the `initialize()` method must always callback any 
registered start-up clients (see below).  Typically this is the only action(s)
required.
```
#include "Cpl/System/Api.h"

namespace Cpl::System;

void Api::initialize( void )
{
    // Init the Colony.Core sub-systems
    StartupHook_::notifyStartupClients();
}
```

The implementation of  `enableScheduling()` and `isSchedulingEnabled()` methods
are very target specific.  For example, for FreeRTOS these method make calls
into the RTOS.  On Windows and Linux these method are essentially NOPs (where 
`isSchedulingEnabled()` always returns true).

The two _sleep_ methods are once again impacted by the Cpl framework's 
[simulated time](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/SimTick.h)
feature. If your target platform is a non-process based RTOS - you typically 
don't have a use case for supporting simulated time - and the implementation for 
the `sleep()` and `sleepInRealTime()` methods are identical. 

If you want/need to support simulated time on your target platform - use the 
[Win32](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Win32) and 
[Posix](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Posix)
implementations a guide/template.

#### Internal Interfaces
The Cpl framework requires a set of mutexes.  While these mutexes are not directly
exposed to the application, the OSAL port still has to provide the mutexes.  The following
[methods](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Private_.h) 
require a platform specific implementation:
```
Mutex& system( void );
Mutex& sysLists( void );
Mutex& tracing( void );
Mutex& tracingOutput( void );
```

### Threads
The _Threads_ interface is the most involved interface to port.  This is because
each target platform/OS/RTOS has it owns interfaces, semantics, and nuances when
it comes to creating threads, allocating stack space, thread priorities, etc.

In addition, some platforms/RTOS/OSes will have created an initial thread _before_
`main()` is called, some do not.  This is problematic because many of the 
`Cpl::System::Thread` methods (e.g. `getCurrent()`) assume that an instance
of the `Thread` class has been created and associated with the current thread.
A native target thread will not have the instance/association because the
thread was __not__ created using the Cpl `createThread()` method.

Finally, one last complication is that the Cpl OSAL semantics for creating threads 
are: threads are created at run time. _Note_: The OSAL semantics __do__ support 
statically allocating the memory for a thread's stack.

The `Thread` interface is defined as an abstract class that also contains several
static methods (see below). The OSAL port requires a concrete child class as
well as implementation for all of the `Thread` static methods.

```
class Thread : public Signable
{
public:
    virtual const char* getName() noexcept = 0;
    virtual size_t getId() noexcept = 0;
    virtual Cpl_System_Thread_NativeHdl_T getNativeHandle( void ) noexcept = 0;
    ...
    static Thread& getCurrent() noexcept;
    static void wait() noexcept;
    ...
    static Thread* create( Runnable&   runnable,
                           const char* name,
                           int         priority      = CPL_SYSTEM_THREAD_PRIORITY_NORMAL,
                           int         stackSize     = 0,
                           void*       stackPtr      = 0,
                           bool        allowSimTicks = true );
    static void destroy( Thread& threadToDestroy );
};
```

The definition of `Cpl_System_Thread_NativeHdl_T` is provided in the `mappings_.h` 
header file. For example:
```
#define Cpl_System_Thread_NativeHdl_T   pthread_t
```

#### Create Thread
Because there is some much variation on how an RTOS/OS creates and manage threads,
a given port of the OSAL may not be able support all of the semantics of the
above `create()` - this is okay as long as the deviations are __documented__ 
and are sufficiently functional for your needs on the target. The reason that
it is _okay_ to deviate from the OSAL semantics is that the code that is creating 
threads is typically very platform specific code. Or said another way, it is code
that is not commonly reused across multiple target platforms.

__Note__: It is strongly recommended that the developer doing the port for the
Threading interface be experienced and well versed in the platform's threading
model.

#### Thread Priorities
It seems every OS/RTOS has different scheme for thread priorities. The Cpl
OSAL handles thread priorities be defining a set of macro symbols that
specify the highest, lowest, and nominal priorities values; as well as two symbols 
to increase or decrease priority values.  The port provides the target 
specific values for these constants in the `mappings_.h` header file. For
example:
```
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      31
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       15
#define CPL_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       0

#define CPL_SYSTEM_THREAD_PRIORITY_RAISE_MAP        (1)
#define CPL_SYSTEM_THREAD_PRIORITY_LOWER_MAP        (-1)
```


#### Native thread vs Cpl Thread
For the scenario where the target platform creates an initial thread for the entry
`main()` function to execute in, the port needs to provide a mechanism for converting 
a native target thread to a Cpl thread.  The recommend approach for this is to 
use the Cpl start-up [mechanism](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/PrivateStartup_.h) 
to register a callback that will convert the native thread when `Cpl::System::Api::initialize()`
is called. See the [Win32](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Win32/Thread.cpp), 
[Posix](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Posix/Thread.cpp), and 
[FreeRTOS](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FreeRTOS/Thread.cpp)
ports for examples.

__WARNING__: There are other use cases where _native_ threads can exist in your
application (other than the initial _main_ thread).  For example: you are using a communication stack provided by
your silicon vendor or third-party.  This stack then creates a pool of worker
threads from which callbacks into your application are made.  This means that
your code in this callbacks can __not__ directly or indirectly call any of the
static class methods of the `Cpl::System::Thread` interface. For example, you
can not have any Cpl Tracing calls because the Tracing engine calls `getCurrent()` 
to retrieve the executing thread's name).

### Thread Local Storage
The [Thread Local Storage](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Tls.h)
interface consist of the following class.  An instance of this class is created
per TLS instance.
```
class Tls
{
public:
    Tls();
    ~Tls();

    void* get( void );
    void set( void* newValue );

protected:
    Cpl_System_TlsKey_T  m_key;
};
```

The definition of `Cpl_System_TlsKey_T` is provided in the `mappings_.h` header file. For
example:
```
#define Cpl_System_TlsKey_T_MAP   pthread_key_t
```

If the target platform does not natively support TLS - there are many possible
options for implementing the TLS interface. For example, one option is to include the TLS
storage as part of the concrete `Thread` class since there is an instance of
the class per thread (e.g. this is how the [FreeRTOS](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/FreeRTOS/Tls.cpp) port is done). Another
example - which is very specific to the [Raspberry PI Pico](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/RP2040/Tls.cpp) port - is to allocate
the TLS storage per CPU core since threads map one to one with cores.

### Tracing
The [Tracing](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/Trace.h)
interface is another interface that in addition to being target specific, it
can _also_ be be application specific. For this reason it is strongly recommended
that the OSAL port provide a default implementation for the Tracing interface _and_
allow for applications (and build scripts) to provide their own implementation. 

For example the Cpl library provides a default implementation for the __entire__
Tracing interface that relies on the [`Cpl::Io::StdOut`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Io/Stdio/StdOut.h) interface. The default 
implementation is segregated into two directories (one for [formating the output](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/_trace/TracePlatform.cpp)
and one for the [target output stream](https://github.com/johnttaylor/pim/blob/master/src/Cpl/System/_trace/_stdout/TraceOutput.cpp) to allow an application to use part, all,
or none of the default implementation).

The _Tracing_ interface has the following methods that require platform/application
specific implementation.  
```
From src/Cpl/System/Trace.h
---------------------------   
void redirect_( Cpl::Io::Output& newMedia );
void revert_( void );
Cpl::Io::Output* getDefaultOutputStream_( void ) noexcept;


From src/Cpl/System/Private_.h
-------------------------------
void appendInfo( Cpl::Text::String& dst, 
                 Trace::InfoLevel_T info, 
                 const char*        section, 
                 const char*        filename, 
                 int                linenum, 
                 const char*        funcname );

void output( Cpl::Text::String& src );
```
