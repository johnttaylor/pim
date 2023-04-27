NQBP stands for (N)ot (Q)uite (B)env - (P)ython.

NQBP (Gen2) is a multi-host build engine designed for:

- C, C++, and assembler builds
- Embedded development (but not limited to)
- Minimal overhead to the developer, i.e. no makefiles required
- Speed.  Uses [Ninja](https://ninja-build.org/) as the underlying build system
- Full dependency checking and incremental builds
- Command line based
- Supporting many Compiler toolchains
- Source code resusablilty, i.e. NQBP assumes that code is/will be shared across many projects.
- Resusablilty of Compiler toolchains, i.e once a particular compiler toolchain has been created/defined - it can be reused across an unlimited number of projects.
- Very effective on all sizes of projects


Gen2 NQBP was created to specially address the lack of dependency checking in _classic_
[NQBP](https://github.com/johnttaylor/nqbp).  This was done by incorporating the
[__Ninja__](https://ninja-build.org/) build tool to perform the actual builds.

Documentation is located in the `top/` directory.

NQBP is licensed with a BSD licensing agreement (see the `top/` directory).

## Differences between NQBP classic and NQBP Gen2
### Front End
The __front end has not__ change. How to specify what source code to build 
(e.g `libdirs.b`, `sources.b`, etc.) and how project specific build flags and options 
are specified (e.g. `mytoolchain.py`) has not changed.

### Compatibility
NQBP Gen2 is __mostly backwards compatible__ with NQBP classic.  In this case 
_mostly_ means that any existing project/test that uses NQBP classic can use
NQBP Gen2 __without__ modifying any of the files NQBP files (e.g. `nqbp.py`,
`mytoolchain.py`, `libdirs.b`, etc.) in a project/test directory.  All that
this is required is that the `NQBP_BIN` path be set to point to the NQBP Gen2
directory.  __However__, you will be required to port your custom toolchain files
to work with NQBP Gen2.

__Note__: You will need to install [ninja](https://ninja-build.org/) on your PC 
and update the command path to include the ninja directory.  Binaries are
available [here](https://github.com/ninja-build/ninja/releases)

To port an existing custom toolchain you will need to be familiar with _ninja_
tool and how it works (but you don't need to be expert or even advanced user
of ninja).  

Here is the list of variables in the toolchain _class_ that have changed in their 
usage and/or expected content.
```
self._link_output           // Need to remove specifying the output file
self._ar_opts               // Need to remove specifying the output file
self._base_release.cflags   // Need to remove -D BUILD_TIME_UTC
self._ar_out                // New
self._rm                    // New
self._shell                 // New
```

Use the following Gen2 toolchains as a reference for when porting your toolchain.
- [Visual Studio](https://github.com/johnttaylor/nqbp2/blob/main/nqbplib/toolchains/windows/vc12/console_exe.py)
- [ARM Cortex M0 (Windows Host)](https://github.com/johnttaylor/nqbp2/blob/main/nqbplib/toolchains/windows/arm_gcc_rp2040/stdio_serial.py)

### Derived Object
NQBP Gen2 now generates/locates __all__ build/derived artifacts (e.g. `.o`, 
`.a`, `.exe`, `.bin`, etc.) under the build variant directory (e.g. `_win32`, `_posix64`, `_pico`, etc.).  

NQBP classic only stored the final outputs (`.exe`, `.bin`) in the build variant 
directory. It stored/located the object files and libraries in the project 
directory itself or in source-path-specific sub-directories.

### BUILD_TIME_UTC
NQBP classic defined the symbol `BUILD_TIME_UTC` as a compiler
preprocessor definition. The value was/is set to UTC time in seconds of when the 
build is performed.  This was done for all builds.  However, this approach 
does not work with incremental builds because the compiler arguments are always 
different (due to new current time value) - which in turn triggers a full build. 

NQBP Gen2 still defines the `BUILD_TIME_UTC` symbol, but by default sets its 
value to `0`.  The Gen2 `nqbpy.py` script has a new option - `--bldtime` - that 
when used, sets `BUILD_TIME_UTC` to the current build time (instead of `0`).  The 
intent here is that formal builds would use the `--bldtime` switch, and developer 
build would not.

### Window's Command Line Length Limitation
__TL;DR__ NQBP Gen2 toolchains includes using compiler _response files_ to avoid the Windows
Command Line length limitation.

A Windows terminal (i.e. `cmd.exe`) command line length is limited to ~8K.  This
can be problematic with large projects or even small projects that have excessive
compiler/linker arguments or numerous header paths (see [here] about my rant about
header file pollution).  The solution for this is use the compiler's _response file_
mechanism for passing arguments. 

The tricky bit comes when using a GCC cross compiler (e.g. ARM7 Cortex) on a
Windows host.  The Windows variant of these compiler properly handle the 
differences in the directory separator (`\` vs `/`) when the argument is passed
on the command line.  However, when compiler is parsing a _response file_ - it
expects/only-handles the `/` as the directory separator.

Using response files for Window Host builds is matter of how the toolchain file
are constructed, i.e. not a limitation of NQBP classic.  However, since the
existing toolchains where being updated for Gen2 - these toolchains have been
modified to use _response files_.

### Command Line Options
NQBP Gen2 has fewer command line options, i.e. eliminated all of the _classic_
options to perform manual incremental builds. In addition the `-t,--turbo` option
has been removed because ninja's default model is build everything in parallel. If
you have any scripts (e.g. CI build scripts) that invoked NQBP with any of the
deprecated command line options - you will need to update those scripts.  Type
`nqbpy.py -h` to get the list of currently supported command line options.

__Note__: The `-1` option (performs a sequential build) is still available in Gen2.


### Performance
NQBP Gen2 is faster on a build-all than NQBP classic.  In my tests Gen2 ranges 
between 10% to 50% faster - depending on the size of the project, PC specs, CI
build vs developer build, etc. Developer builds see the biggest performance
increase because there is less console output/logging.

### TODO
The toolchains listed below have been ported to NQBP Gen2 - but I have not verified
the toolchains (I no longer have the compilers installed on my PC).  In addition,
these toolchains need to be updated to use response files when building.

- `nqbp2\nqbplib\toolchains\windows\arm_m4_arduino\nrf52_feather52.py`
- `nqbp2\nqbplib\toolchains\windows\avr_gcc_arduino\atmega328p_uno.py`
