#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

#define OPTION_CPL_SYSTEM_SIM_TICK_DEFAULT_NUM_ITERS	3
#define USE_CPL_SYSTEM_SIM_TICK

// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif


#endif
