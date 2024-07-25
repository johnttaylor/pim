/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014, 2015  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "colony_config.h"
#include <stdlib.h>
#include <ios>
#include "FreeRTOS.h"


/** If using one of FreeRTOS's heap strategies/options that supports freeing
    memory - THEN define this symbol
 */
#ifdef USE_FULL_FEATURED_HEAP
#define rtosFree    vPortFree

#else
/** The default is to using a heap that does NOT support freeing memory
 */
#define rtosFree(p)     // Ignore the delete/free call
#endif


///// Provide a non-exception-throwing new/delete so exception code does NOT get
///// linked in (i.e. -fno-exception is NOT enough to get rid of exception code
///// from libstdc++.a)
//
//
void *operator new( size_t size, std::nothrow_t const& ) throw() { return pvPortMalloc(size); }
void *operator new[]( size_t size, std::nothrow_t const& ) throw() { return pvPortMalloc(size); }

void operator delete(void* p) throw() { rtosFree( p ); }
void operator delete[]( void* p ) throw() { rtosFree( p ); }

const std::nothrow_t std::nothrow;
//
//
///* The function __cxa_atexit() handles the static destructors. In a bare-metal 
//   system this function can be empty because application has no operating system 
//   to return to, and consequently the static destructors are never called.
//*/
extern "C" int __cxa_atexit( void ) { return 0; }
//
////extern "C" int __cxa_atexit( void* object,
////                             void (*destructor)(void*),
////                             void* dso_handle
////                           )
////    {
////    return 0;
////    }
//
extern "C" void __cxa_pure_virtual() { while ( 1 ); }

/*
** Method stubs to satisfy the linker -->NOTE: C++ Streams are NOT supported by this BSP
*/
std::ios_base::Init::Init()
{
}

std::ios_base::Init::~Init()
{
}
