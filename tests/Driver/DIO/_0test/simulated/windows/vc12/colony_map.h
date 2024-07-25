
// strapi mapping
#include "Cpl/Text/_mappings/_vc12/strapi.h"

// Driver
#include "Driver/DIO/Simulated/mappings_.h"

#ifdef BUILD_VARIANT_WIN32
// Cpl::System mappings
#include "Cpl/System/Win32/mappings_.h"
#endif


#ifdef BUILD_VARIANT_CPP11
// Cpl::System mappings
#include "Cpl/System/Cpp11/_win32/mappings_.h"
#endif

