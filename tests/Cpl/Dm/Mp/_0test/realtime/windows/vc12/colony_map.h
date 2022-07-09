#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_


// strapi mapping
#include "Cpl/Text/_mappings/_vc12/strapi.h"


#ifdef BUILD_VARIANT_WIN32
// Cpl::System mappings
#include "Cpl/System/Win32/mappings_.h"
#endif


#ifdef BUILD_VARIANT_CPP11
// Cpl::System mappings
#include "Cpl/System/Cpp11/_win32/mappings_.h"
#endif




#endif
