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
#include "pico/cyw43_arch.h"
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

    // Initialize the Wifi chip
    int err = cyw43_arch_init();
    if ( err )
    {
        printf( "**ERROR: WiFi init failed: %d", err );
    }
}

static bool debugLed1State_;
void Bsp_Api_turnOnDebug1()
{
    debugLed1State_ = true;
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, debugLed1State_ );
}

void Bsp_Api_turnOffDebug1()
{
    debugLed1State_ = false;
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, debugLed1State_ );
}

void Bsp_Api_toggleDebug1()
{
    debugLed1State_ = !debugLed1State_;
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, debugLed1State_ );
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