#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable Trace
#define USE_CPL_SYSTEM_TRACE

// Force DOS newline
#define USE_CPL_IO_NEW_LINE_STANDARD   CPL_IO_NEW_LINE_NATIVE

// Set native newline for C++11 platform
#ifdef BUILD_VARIANT_CPP11
#define CPL_IO_NEW_LINE_NATIVE_MAP              "\015\012"
#endif

#endif
