#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable ASSERT macros (part of the expected output)
#define USE_CPL_SYSTEM_ASSERT_MACROS

// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif

#endif
