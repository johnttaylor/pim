#include "FreeRTOS.h"
#include "task.h"
#include "Bsp/Api.h"
#include "Cpl/System/FatalError.h"


#define TIMER_HZ_RATE      1000

/*-----------------------------------------------------------*/
extern "C" void vApplicationSetupTimerInterrupt( void )
	{
    Bsp_initCompareMatchTimer( 0, TIMER_HZ_RATE, 0, configKERNEL_INTERRUPT_PRIORITY );
	Bsp_startCompareMatchTimer( 0 );
	}


/*-----------------------------------------------------------*/
/* This function is explained by the comments above its prototype at the top
of this file. */
extern "C" void vApplicationMallocFailedHook( void )
	{
    Cpl::System::FatalError::logRaw( "vApplicationMallocFailedHook()" );
	}


/*-----------------------------------------------------------*/
/* This function is explained by the comments above its prototype at the top
of this file. */
extern "C" void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
	{
    Cpl::System::FatalError::logRaw( "vApplicationStackOverflowHook()" );
	}


/*-----------------------------------------------------------*/
/* This function is explained by the comments above its prototype at the top
of this file. */
extern "C" void vApplicationIdleHook( void )
	{
	}

