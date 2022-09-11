#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

#define OPTION_CPL_SYSTEM_SIM_TICK_NO_ACTIVITY_LIMIT	250 // .25sec wait
#define USE_CPL_SYSTEM_SIM_TICK

// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif


#endif
