#include "FreeRTOS.h"
#include "task.h"
#include "Bsp/Api.h"

#define TIMER_HZ_RATE      1000

/*-----------------------------------------------------------*/
void vApplicationSetupTimerInterrupt( void )
	{
    Bsp_initCompareMatchTimer( 0, TIMER_HZ_RATE, 0, configKERNEL_INTERRUPT_PRIORITY );
	Bsp_startCompareMatchTimer( 0 );
	}


/*-----------------------------------------------------------*/
/* This function is explained by the comments above its prototype at the top
of this file. */
void vApplicationMallocFailedHook( void )
	{
	for( ;; );
	}


/*-----------------------------------------------------------*/
/* This function is explained by the comments above its prototype at the top
of this file. */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
	{
	for( ;; );
	}


/*-----------------------------------------------------------*/
/* This function is explained by the comments above its prototype at the top
of this file. */
void vApplicationIdleHook( void )
	{
	}

