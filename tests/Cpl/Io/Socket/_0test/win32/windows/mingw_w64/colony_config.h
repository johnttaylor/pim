#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif

// Force DOS newline
#define USE_CPL_IO_NEW_LINE_STANDARD   CPL_IO_NEW_LINE_NATIVE

// Set native newline for C++11 platform
#ifdef BUILD_VARIANT_CPP11
#define CPL_IO_NEW_LINE_NATIVE_MAP              "\015\012"
#endif


#endif
