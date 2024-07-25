/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Bsp/Api.h"
#include "stm32f4xx_hal.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/MX/Core/Inc/gpio.h"
#include "Bsp/ST/NUCLEO-F413ZH/alpha1/console/Output.h"
#include "Cpl/System/Trace.h"

#ifdef ENABLE_BSP_SEGGER_SYSVIEW   
#define INIT_SEGGER_SYSVIEW()   SEGGER_SYSVIEW_Conf()
#else
#define INIT_SEGGER_SYSVIEW()   
#endif

#define SECT_   "bsp"

///////////////////////////////////////////
void Bsp_Api_initialize( void )
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_TIM4_Init();
    MX_USART3_UART_Init();
    MX_I2C2_Init();

    // Initialize System View (asap after the basic board initialization has completed)
    INIT_SEGGER_SYSVIEW();

    // Start the Console/Trace UART
    g_bspConsoleStream.start( USART3_IRQn, &huart3 );
}

