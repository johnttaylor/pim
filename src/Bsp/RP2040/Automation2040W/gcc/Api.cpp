/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Bsp/Api.h"
#include "pico/stdlib.h"
#include <ios>

#define SECT_   "bsp"


critical_section_t g_bspGlobalCritSec_;

#ifdef USE_BSP_NO_STDIO
#define INIT_STDIO()
#else
#define INIT_STDIO  stdio_init_all
#endif

///////////////////////////////////////////
void Bsp_Api_initialize( void )
{
    // Initialize STDIO
    INIT_STDIO();

    // Initialize the global critical section
    critical_section_init( &g_bspGlobalCritSec_ );

    // Configure the LEDs as output pins 
    gpio_init( OPTION_BSP_DEBUG_LED1_INDEX );
    gpio_set_dir( OPTION_BSP_DEBUG_LED1_INDEX, GPIO_OUT );
    gpio_init( OPTION_BSP_DEBUG_LED2_INDEX );
    gpio_set_dir( OPTION_BSP_DEBUG_LED2_INDEX, GPIO_OUT );

}


////////////////////////////////////////////////////
/*
** Method stubs to satisfy the linker -->NOTE: C++ Streams are NOT supported by this BSP
*/
//void std::ios_base::Init::_S_ios_destroy()
//{
//}
//
//void std::ios_base::Init::_S_ios_create( bool  )
//{
//}

std::ios_base::Init::Init()
{
}

std::ios_base::Init::~Init()
{
}

//void *operator new(size_t size, std::nothrow_t const&){
//    return malloc( size );
//}
//
//void *operator new[]( size_t size, std::nothrow_t const& ) {
//    return malloc( size );
//}
//
//const std::nothrow_t std::nothrow;