#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable ASSERT macros	 (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_ASSERT_MACROS
#endif

// Enable Trace 
#define USE_CPL_SYSTEM_TRACE

#endif
