#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable 'TestB'
#define BUILD_TEST_B_

// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif

#endif
