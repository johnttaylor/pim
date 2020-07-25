#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

/////////////////////////
// Define/Set which vectors I am using

// My application's Timer vectors
#define BSP_VECTOR_CMTU1_CMI3   irqTimer1

// FreeRTOS kernel vectors
#define BSP_VECTOR_ICU_SWINT    vSoftwareInterruptISR
#define BSP_VECTOR_CMTU0_CMI0   vTickISR
   
//
/////////////////////////


/////////////////////////
// Prototype for the above Vector funcitons

#ifdef __cplusplus
extern "C" {
#endif

// My app
extern void irqTimer1 (void) __attribute__ ((interrupt));

// FreeRTOS
extern void vTickISR( void ); // __attribute__ ((interrupt));
extern void vSoftwareInterruptISR( void ); // __attribute__ ((interrupt));


#ifdef __cplusplus
}
#endif

//
/////////////////////////

/// My BSP
#include "Bsp/Renesas/Rx/u62n/Yrdkr62n/Gnurx/Api.h"


/// strapi mapping
#include "Cpl/Text/_mappings/kpitrx/strapi.h"


#endif
