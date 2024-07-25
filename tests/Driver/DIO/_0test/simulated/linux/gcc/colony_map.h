
// Cpl::System mappings
#if defined(BUILD_VARIANT_POSIX) || defined(BUILD_VARIANT_POSIX64)
#include "Cpl/System/Posix/mappings_.h"
#endif
#ifdef BUILD_VARIANT_CPP11
#include "Cpl/System/Cpp11/_posix/mappings_.h"
#endif

// strapi mapping
#include "Cpl/Text/_mappings/_posix/strapi.h"

// Driver
#include "Driver/DIO/Simulated/mappings_.h"

