# Introduction to the TShell
The TShell is a C++ framework that provides a text based shell that can be used 
to interact with an Application.  Typical usage is to provide the developer
access for troubleshooting, debugging, and white box testing. However, the
TShell can be used _anywhere_ a text based, command-response, stream oriented
interface is useful.


## Features
The TShell framework is part of the CPL C++ Class library and has the following 
features:

* Text based.  
  * Commands and responses contain printable ASCII characters and use 
    newline to delineated commands.
  * Supports quoted command arguments (including embedded quotes)
* Simple to add new/custom commands.
  * Commands self register with the shell's command list.
* IO is stream based.  
  * e.g. UART, stdio, Sockets, etc.
* Supports blocking and non-blocking read operations
  * Non-blocking variant can be used with bare-metal (no RTOS) applications
* Run time help
  * Includes terse and verbose options
* Supports out-of-bound (OOB) reading of the input stream by individual commands
  * e.g. useful when reading a large amount of data, or reading data that
    contains newlines.
* Optional user login/permission levels
  * Default is security off, i.e. all access
  * 4 levels of permission/access supported
  * Validation of login credentials is application specifc
* Can be started, stopped, and restarted for use cases where the IO streams are
  dynamic (e.g. TCP sockets).
* Supports atomic output with respect to other sub-systems
  * e.g. prevents _interleaving_ shell response outputs with Trace outputs.
* No dynamic memory allocation required.
* Platform independent
  * Built on top the CPL C++ Class Library's OSAL interfaces
* Thread safe
  * The Framework is fully thread safe.  
  * The application is responsible for thread safety of its custom commands. 
* Customizable:
  * Prompt string
  * Token separator
  * Quote and Escape characters
  * Maximum command length
  * Maximum output line length
    * This is a per response line setting (i.e. does not limit the number of lines a response can output)
  * Greeting and Farewell banners 
  * Comment character
  * Start and End of Frame characters (SOF, EOF)
* Set of basic commands provided:
  * `help` - displays command help
  * `threads` - displays the current threads
  * `trace` - Controls the Trace output
  * `bye` - Exit the application and/or the shell
  * `dm` - Read/write access to Model points
  * `tick` - Provides the _tick timing_ when an Application is built using 
     simulated time
  * `user` - Login/logout command (for when security is enabled)
  
### Not Included
Features __not__ included in shell framework
* Command echoing. Input characters are not echoed back to the sender
  * e.g. when using [Putty](https://www.putty.org/), enable _Local Echo_
* Command line editing, i.e. the backspace char (^H) is not interpreted 
  * e.g. when using [Putty](https://www.putty.org/), enable _Local Line Editing_
* Asynchronous notifications.  The shell only responds to commands, it never
  initiates a _transaction_.

# What's a Command
Commands have the following syntax:

* A command starts with a printable ASCII character and ends with
  a newline.

* Non-printable ASCII characters are not allowed.

* Any line that starts with a `#` is treated as a comment line and
  is ignored.

* Format of a command is: `verb [arg]*`  where the `verb` and `arg`
  are separated by spaces.  Arguments can contain spaces character
  by enclosing the argument with double quote (`"`) characters.  A double
  quote character can be embedded inside a quoted string by preceding
  the double quote character with the escape character (`` ` ``).  The escape
  character can be embedded by escaping the escape character.  Examples
  of quoted arguments:
  * `mycmd arg1 "this is arg2" arg3`
  * ``mycmd "arg1 `"bob`" is quoted" arg2 arg3``
  * ```mycmd "arg1 embedded ``escape`` characters" arg2 arg3```

* A command can output zero or more lines when it executes. Each line of output
  is an atomic operation for the shell.

Example __threads__ command:
```
$ help threads
threads
  Displays the list of threads.

$ threads

Name              R  ID        Native Hdl  Pri  User Time     Kernel Time
----              -  --        ----------  ---  ---------     -----------
Win32Main         Y  000067E8  000000F0      0  00:00:00.031  00:00:00.000
Application       Y  00006D80  00000114      0  00:00:00.078  00:00:00.031
TShell            Y  00005D34  00000128     -2  00:00:00.000  00:00:00.000

Total number of threads: 3
$
```
  
Example __trace__ command:
```
$ help trace
trace [on|off]
trace section (on|off) <sect1> [<sect2>]...
trace threadfilters [<threadname1> [<threadname2>]]...
trace level (none|brief|info|verbose|max)
trace here|revert
  Enables/Disables the Cpl::System::Trace engine and manages the section',
  information level, and thread filter options.  See the Cpl::System::Trace
  interface for details on how the trace engine works.

$ trace section on Cpl::TShell
$ trace

TRACE: Currently Enabled Sections:
----------------------------------
Cpl::TShell

TRACE: Currently Enabled Thread Filters:
----------------------------------------

TRACE: Runtime state:= ENABLED, Info Level:= BRIEF
$
```
# How it Works
The shell contains a list of commands.  Each command in the list has a method
that return its _command verb_ (aka the command name) and a method to be 
executed when the command is received by the shell.

The shell requires at run-time:
* A input stream that it accepts commands/input from. 
* A output stream where it outputs text to in response to commands.

The stream interfaces are defined by the [Cpl::Io namespace](https://github.com/johnttaylor/pim/blob/master/src/Cpl/Io)

The shell hasd a command/response semantics, i.e. the User sends a command, the
shell accepts (or rejects) the command - and the command optionally responds with text
via the output stream. 

The command text in the input stream is identified by SOF and EOF markers.  The 
SOF and EOF markers are customizable when creating a shell instance.  However, 
the default SOF marker is _any_ printable ASCII character and EOF is a newline 
(`\n`).  

When a command text frame is detected, the first word in the frame is compared against 
the shell's list of command verbs.  If there is a match (and user has the required
permissions), the shell calls the command's execute method.  Commands 
return a success/error code upon completion. If an error occur, the shell outputs 
error messages. 

After a command has completed executing, the shell resuming scanning the input
stream. This behavior repeats until the shell self terminates or the application 
exits.



# Creating a Command
A Command is a concrete C++ class that implements the `Cpl::TShell::Command` 
interface. In addition, each concrete class is required (in its constructor) to 
_self register_ with the shell's command list. The following code snippet 
lists the methods that each command is required to implement.

```
/// This method executes the command.
virtual Result_T execute( Context_&        context, 
                          char*            rawCmdString, 
                          Cpl::Io::Output& outfd ) noexcept = 0;

/// This method returns the command's verb string
virtual const char* getVerb() const noexcept = 0;

/// This method returns the command's usage string
virtual const char* getUsage() const noexcept = 0;

/** This method returns the command's detailed help string.  Detailed
    help is optional.  If the command does not support detailed help,
    then nullptr is returned.
 */
virtual const char* getHelp() const noexcept = 0;

/** Returns the minimum required permission level needed to execute the 
    command.
 */
virtual Security::Permission_T getMinPermissionRequired() const noexcept = 0;
```

In practice there is `Cpl::TShell::Cmd::Command` base class that implements 
the self registration of the command with the command list along with some 
other (minimal) boiler plate code.
  
# Summary
If you have never used a _debug console_ of some kind on your embedded 
projects, all I can say is that once you have used one, you will never want to 
do another project without one.  Somewhat analogous to the world before and
after microwave ovens ;-).

The TShell framework is located [here](https://github.com/johnttaylor/pim/blob/master/src/Cpl/TShell)

The basic set of TShell commands is located at [here](https://github.com/johnttaylor/pim/blob/master/src/Cpl/TShell/Cmd)

A TShell example project is located at [here](https://github.com/johnttaylor/pim/blob/master/projects/Examples/TShell/README.md)