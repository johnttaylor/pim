# Data Model Example

The Data Model example project is an introduction to using the Data Model framework.  A brief overview to the data model can be found [here](https://github.com/johnttaylor/pim/blob/master/README-Intro-DataModel.md). 

The example illustrates the following:
- Multi-threaded application
- Decoupling 'drivers' from the Application logic
- Creating an application specific model point
  - Includes customized read-modify-write operations for a model point
- Polled semantics for reading values sourced by the input driver
- The output driver uses event semantics
- Persistent storage of model points
- Console commands to interrogate and modify model points


### Example Application
The example application contains the following modules/classes.  __Note:__ the
goal of the application is to illustrate how to use model points, not to do
anything that is useful ;-).

- __InputDriver__.  The Input Driver is representative of an 'input driver' that
samples a physical signal.  The driver generates a random value in the range of 1 to 1000.
The Input Driver executes in the _Driver_ thread and generates values at 100Hz.
  - Files: [`InputDriver.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/InputDriver.h), [`InputDriver.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/InputDriver.cpp)

- __OutputDriver__.  The Output Driver is representative of an 'output driver' that
drives physical output signals. The driver asserts/de-asserts two Boolean outputs, 
one signal each for the High and Low Alarms. The Output Driver executes in the _Driver_ 
thread and only changes its output values when the Alarms change state.
  - Files: [`OutputDriver.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/OutputDriver.h), [`OutputDriver.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/OutputDriver.cpp)

- __Algorithm__.  The Algorithm module is representative of processing that is done
on the Application's inputs and generates outputs per the Application requirements. 
The Algorithm module executes periodically (at 10Hz) in the _Application_ thread. The
Algorithm module does the following:

  - Captures the number of samples, cumulative sum, minimum, and maximum of its 
    input value.  These metrics are stored in the MpMetrics model point.
  - Captures the number of times the High and Low alarms are asserted.  These
    counts are stored in individual model points.
  - Generates a 'High Alarm' when the current input value goes above the value 
    of 995.  The Alarm is lowered once the current value drops below 5.
  - Generates a 'Low Alarm' when the current input value goes below the value 
    of 5.  The Alarm is lowered once the current value goes above 995.

  - Files: [`Algorithm.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/Algorithm.h), [`Algorithm.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/Algorithm.cpp)

- __MpAlarm__.  The MpAlarm class defines an application specific model point
used for managing a Alarm.
  - Files: [`MpAlarm.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MpAlarm.h), [`MpAlarm.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MpAlarm.cpp)

- __MpMetrics__.  The MpMetrics class defines an application specific model point
used by the Algorithm module to store its metrics.
  - Files: [`MpMetrics.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MpMetrics.h), [`MpMetrics.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MpMetrics.cpp)

- __MetricsRecord__.  The Metrics Record header file defines the persistent storage
record for the Application.  The Algorithm's metrics, alarm counters, and the boot counter
model points are included in the persistent record. The Metrics Record executes 
in the _Persistence_ thread. Note: The Metrics Record is
responsible for initializing/incrementing the boot counter on start-up. 
  - Files: [`MetricsRecord.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MetricsRecord.h)

- __ModelPoints__.  The Model Points module defines _all_ of the model points used by 
the Application.  All of the model points are statically allocated in the invalid state.
  - Files: [`ModelPoints.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/ModelPoints.h), [`ModelPoints.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/ModelPoints.cpp)

- __TShell Console__.  The TShell console (think debug console) from the Cpl 
C++ Class library is included to allow the developer to read and write model 
points at run time.
  - Directory: [`Cpl/TShell/`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/TShell)
- __Persistent Storage__. The persistent storage framework from the CPL C++ 
Class library is included to store the Metrics Record.  The data persistently
stored is CRC'd at rest in order to detect corrupt records/data.  The persistent
storage is read into RAM at start-up and the updated to NVRAM 'on-demand' as the
application runs.
  - The persistence data is stored in the file `nvram.bin` located in the current 
    working directory of where the example executable was launched.  Deleting (or
    editing) this file will trigger a _corrupt storage_ use case and cause the
    persistent data to be reset to its default values as defined by application.
  - Directories: [`Cpl/Persistent/`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Persistent), [`Cpl/Dm/Persistent/`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Dm/Persistent)

- __Main__.  The Main module is responsible for constructing all of the modules
and/or classes along with the start-up and shutdown logic.  And orderly shutdown 
of the application is triggered by the calling the `Cpl::System::Shutdown::success()` 
method.  There is a total of five threads in the application:

  | Thread | Description |
  |--------|-------------|
  | xxxMain     | This the main thread of the executable's process.  The start-up/shutdown logic executes in this thread. |
  | Application | This the primary _application_ thread and it is event/message based thread. The `Algorithm` class executes in this thread. |
  | Drivers     | This thread is for the execution of the _drivers_ and it is event/message based thread.  This thread has the highest priority. |
  | Persistence | This is an event/message based thread used to perform the physical read/writes to NVRAM. This thread has lower priority than the Application thread. |
  | TShell      | The TShell console runs in own dedicate thread.  This thread has the lowest priority. |

  - Files: [`Main.h`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/Main.h), [`Main.cpp`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/Main.cpp)
### Model Points
| Model Point | Type | Description |
|-------------|:----:|-------------|
| `mp::signalIn`    | `Cpl::Dm::Mp::Uint32`     | Input Signal.  Range is 1 to 1000|
| `mp::metrics`     | `MpMetrics`                | Algorithm generated metrics for the input signal |
| `mp::bootCounter` | `Cpl::Dm::Mp::Uint32`    | Number of the times the application has been run |
| `mp::hiAlarm`     | `MpAlarm`       | High Alarm.  When the alarm is _asserted_ the model point value is __valid__.  When the alarm is _de-asserted_ the model point is __invalid__ | 
| `mp::loAlarm`     | `MpAlarm`       | Low Alarm.  The low alarm has the same semantics/value range as the High Alarm| 
| `mp::hiAlarmCounts`     | `Cpl::Dm::Mp::Uint32`       | The cumulative number of times the High Alarm has been raised| 
| `mp::loAlarmCounts`     | `Cpl::Dm::Mp::Uint32`       | The cumulative number of times the Low Alarm has been raised| 

### Model Point APIs
This section provides links to the Model Point Headers files.

- [`Cpl::Dm::ModelPoint`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Dm/ModelPoint.h). This is the API class that defines all of the common operation that 
  can be performed on model point. 
  - [`Cpl::Dm::ModelPointCommon_`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Dm/ModelPointCommon_.h). This is the type-independent implementation base class that provides the majority of functionality for model points.
- [`Cpl::Dm::Mp::Uint32`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Dm/Mp/Uint32.h). This is the type specific MP for containing `uint32_t` data type.
  - [`Cpl::Dm::Mp::Numeric`](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Dm/Mp/Numeric.h). This file contains a collection of template classes that provides much of the `Uint32` implementation.
- [`MpAlarm`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MpAlarm.h). This is the application specific MP for alarm data. 
- [`MpMetrics`](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/MpMetrics.h). This is the application specific MP for metric data. 



### TShell Console
The TShell is a text-based command shell framework that can be used to interact 
with an application. One example is a debug shell (or maintenance port) that 
provides white-box access to running the application. Note: the TShell is only 
a framework; the application is responsible for connecting it to the application 
and providing application specific commands.

The TShell has a `help` command that lists all of the available commands.  Typing
`help *` or `help <cmd>` provides additional details for the commands.  For example:

```
$ help
bye [app [<exitcode>]]
dm ls [<filter>]
dm write {<mp-json>}
dm read <mpname>
dm touch <mpname>
help [* | <cmd>]
threads
tprint ["<text>"]
trace [on|off]
trace section (on|off) <sect1> [<sect2>]...
trace threadfilters [<threadname1> [<threadname2>]]...
trace level (none|brief|info|verbose|max)
trace here|revert

$ help dm
dm ls [<filter>]
dm write {<mp-json>}
dm read <mpname>
dm touch <mpname>
  Lists, updates, and displays Model Points contained in the Model Database.
  When 'ls' is used a list of model point names is returned.  The <filter>
  argument will only list points that contain <filter>.  Updating a Model Point
  is done by specifying a JSON object. See the concrete class definition of the
  Model Point being updated for the JSON format.  When displaying a Model Point
  <mpname> is the string name of the Model Point instance to be displayed.
```

Below are example TShell commands that will read, write, and then read the `hiAlarm` model point

```
>> 00 00:15:41.932 (OutputDriver) hiAlarm RASIED. Signal=997, Acknowledged=no
$ dm read hiAlarm
{
  "name": "hiAlarm",
  "valid": true,
  "type": "MpAlarm",
  "seqnum": 38,
  "locked": false,
  "val": {
    "signal": 997,
    "ack": false
  }
}
$ dm write {name:"hiAlarm",val:{ack:true}}
>> 00 00:15:43.313 (OutputDriver) hiAlarm RASIED. Signal=997, Acknowledged=YES
$ dm read hiAlarm
{
  "name": "hiAlarm",
  "valid": true,
  "type": "MpAlarm",
  "seqnum": 39,
  "locked": false,
  "val": {
    "signal": 997,
    "ack": true
  }
}
$
```
### Installation and Setup
The source code for the Data Model example is part of the [PIM](https://github.com/johnttaylor/pim) repository on GitHub. The example code is functional on
both a Windows and a Linux PC.

Here are the minimum prerequisites for both Windows and Linux:
- Python 3.6 or newer.
- At least one compiler installed.  Supported compilers are:
  - GCC under Linux
  - Visual Studio VC12 (or higher) under Windows
  - MinGW GCC Compiler under Windows.
- Git client installed. This is optional if, instead of cloning the repository, 
  you want to work with a downloaded zip file from GitHub.

Once the everything is installed/downloaded to your box, open a terminal window
and navigate to the root of your local GIT repository and run the `env.bat` 
or `env.sh` script to setup the compiler toolchain.  For example under Windows 
using the VC compiler:

```
c:\_workspaces\zoe\pim>env
NO TOOLCHAIN SET

1 - GCC-ARM compiler for Grand Central BSP 1.5.7
2 - GCC-ARM compiler for Grand Central BSP 1.6.0
3 - GCC-AVR cross compiler for Atmel AVR/arduino
4 - GCC-RX cross compiler for Renesas RX
5 - MINGW64 v9.2.0 compiler for Windows
6 - GCC-ARM Cortex M/R cross compiler for NRF52/Arduino
7 - Visual Studio VC16 (32bit) compiler for Windows
8 - Visual Studio VC16 (64bit) compiler for Windows

c:\_workspaces\zoe\pim>env 7
**********************************************************************
** Visual Studio 2019 Developer Command Prompt v16.8.3
** Copyright (c) 2020 Microsoft Corporation
**********************************************************************
[vcvarsall.bat] Environment initialized for: 'x86'

c:\_workspaces\zoe\pim>
```

__Notes__: 
- Running the `env.bat|sh` script only needs to be run once per terminal window/session.
- All builds are command line based and use the PIM repository's `nqbp` build engine.

### Building the Example Application
The PIM repository directory structure separates the source code directories from 
the directories where the builds are performed. In the following top-level directory
structure, application builds and unit tests are built in the `projects/` and `tests/` 
directory trees, respectively. The source code resides under the `src/` or `xsrc/`
directory trees.
```
<workspace root>
├─── docs
├─── projects   ; Build applications/released images
├─── resources
├─── scripts
├─── src
├─── tests      ; Build unit tests
├─── top
└─── xsrc       ; Contains the NQBP build engine (among other things)
```

The Application specific source code the example application is located in 
`project/Examples/DataModel/` directory. The location of the example source 
code does __not__ follow the paradigm for locating application source code under 
the `src/` directory.  This exception was done to simplify the example. See the projects
under the `projects/Storm/` directory for the recommended file/directory structure.

To build the example application
navigate to a leaf directory inside of the `project/Examples/DataModel/` 
directory and run the `nqbp.py` script. Which leaf directory depends on which 
host-platform/compiler you are using. 

The following is an example of building and running the executable on Windows using the 
Visual Studio compiler (note: the `-g -t` options enable the debug and parallel
build options respectively).

```
c:\_workspaces\zoe\pim>cd projects\Examples\DataModel\windows\vc12
c:\_workspaces\zoe\pim\projects\Examples\DataModel\windows\vc12>nqbp.py -gt
================================================================================
= START of build for:  datamodel-example.exe
= Project Directory:   C:\_workspaces\zoe\pim\projects\Examples\DataModel\windows\vc12
= Toolchain:           VC++ 12, 32bit (Visual Studio 2013)
= Build Configuration: win32
= Begin (UTC):         Sun, 25 Dec 2022 20:51:48
= Build Time:          1672001508 (63a8b7e4)
================================================================================
= Cleaning Built artifacts...
= Cleaning Project and local Package derived objects...
= Cleaning External Package derived objects...
= Cleaning Absolute Path derived objects...
=====================
= Building Directory: src\Cpl\Dm\Persistent
=====================
= Building Directory: src\Cpl\Persistent
=====================
= Building Directory: src\Cpl\Text
Pool_.cpp
TextBlock.cpp
Basic.cpp
Decoder_.cpp
cdlist.c
=
= Archiving: library.lib

...

StdOut.cpp
=
= Archiving: library.lib
=====================
= Building Project Directory:
Algorithm.cpp
InputDriver.cpp
OutputDriver.cpp
MpMetrics.cpp
MpAlarm.cpp
ModelPoints.cpp
Main.cpp
winmain.cpp
=====================
= Linking...
================================================================================
= END of build for:    datamodel-example.exe
= Project Directory:   C:\_workspaces\zoe\pim\projects\Examples\DataModel\windows\vc12
= Toolchain:           VC++ 12, 32bit (Visual Studio 2013)
= Build Configuration: win32
= Elapsed Time (hh mm:ss): 00 00:13
================================================================================

c:\_workspaces\zoe\pim\projects\Examples\DataModel\windows\vc12>_win32\datamodel-example.exe
>> 00 00:00:00.009 (OutputDriver) hiAlarm lowered.
>> 00 00:00:00.009 (OutputDriver) loAlarm lowered.

--- Your friendly neighborhood TShell. ---


$ dm ls
bootCounter
hiAlarm
hiAlarmCounts
loAlarm
loAlarmCounts
metrics
signalIn
$ 
>> 00 00:00:06.046 (OutputDriver) hiAlarm RASIED. Signal=1000, Acknowledged=no
>> 00 00:00:08.213 (OutputDriver) hiAlarm lowered.
>> 00 00:00:08.367 (OutputDriver) loAlarm RASIED. Signal=3, Acknowledged=no
>> 00 00:00:11.359 (OutputDriver) hiAlarm RASIED. Signal=996, Acknowledged=no
>> 00 00:00:11.359 (OutputDriver) loAlarm lowered.
>> 00 00:00:21.354 (OutputDriver) hiAlarm lowered.
>> 00 00:00:21.354 (OutputDriver) loAlarm RASIED. Signal=1, Acknowledged=no
$ dm read bootCounter
{
  "name": "bootCounter",
  "valid": true,
  "type": "Cpl::Dm::Mp::Uint32",
  "seqnum": 2,
  "locked": false,
  "val": 1
}
$
```