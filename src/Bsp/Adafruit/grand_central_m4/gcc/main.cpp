/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "colony_config.h"

#define ARDUINO_MAIN
#include "Arduino.h"
#include "FreeRTOS.h"
#include "task.h"
#include <ios>
#include <new>

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

// Initialize C library
extern "C" void __libc_init_array(void);

static void main_task( void *p );

// Stack size in BYTES (not StackType_T)
#ifndef OPTION_MAIN_THREAD_STACK_SIZE   
#define OPTION_MAIN_THREAD_STACK_SIZE               ((2*1024) >> (sizeof( StackType_t ) / 2)) 
#endif

// Main thread priority
#ifndef OPTION_MAIN_THREAD_PRIORITY
#define OPTION_MAIN_THREAD_PRIORITY                 (configMAX_PRIORITIES/2)
#endif




/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
    init();

    __libc_init_array();

    initVariant();

    // NOTE: Not compatible with TinyUSB
    USBDevice.init();
    USBDevice.attach();

    // Create main thread
    if ( xTaskCreate( main_task, "MAIN", OPTION_MAIN_THREAD_STACK_SIZE, NULL, OPTION_MAIN_THREAD_PRIORITY, 0 ) != pdPASS )
    {
        while ( 1 );
    }

    vTaskStartScheduler();
    return 0;
}

static void main_task( void *p )
{
    setup();

    for ( ;;)
    {
        loop();
    }
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

// NOTE: The FreeRTOS heap is an allocate heap only (heap1.c) -->so no need to overload the delete the methods
void *operator new( size_t size, std::nothrow_t const& )
{
    return pvPortMalloc( size );
}

void *operator new[]( size_t size, std::nothrow_t const& )
{
    return pvPortMalloc( size );
}

const std::nothrow_t std::nothrow;