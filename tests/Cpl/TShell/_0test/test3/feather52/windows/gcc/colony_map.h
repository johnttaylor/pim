#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

/////////////////////////
// CPL Stuff....
#include "Cpl/System/FreeRTOS/mappings_.h"
  
/* Arduino Thread Priorities
  TASK_PRIO_LOWEST  = 0, // Idle task, should not be used
  TASK_PRIO_LOW     = 1,
  TASK_PRIO_NORMAL  = 2, // Loop, Timer Task
  TASK_PRIO_HIGH    = 3, // Bluefruit Task
  TASK_PRIO_HIGHEST = 4,
*/

// Change the thread priorities to be compatible with the Arduino thread priorities
#undef  CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       
#define CPL_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       1   // aka TASK_PRIO_LOW
#undef  CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP
#define CPL_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      4   // aka TASK_PRIO_HIGHEST


// strapi mapping
#include "Cpl/Text/_mappings/_arm_m4_arduino/strapi.h"


//
/////////////////////////

// My BSP
#include "Bsp/Adafruit/feather52/gcc/Api.h"



#endif
