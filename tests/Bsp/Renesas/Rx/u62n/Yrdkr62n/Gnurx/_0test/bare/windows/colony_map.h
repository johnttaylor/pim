#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

/////////////////////////
// Define/Set which vectors I am using

// Timer vectors
#define BSP_VECTOR_CMTU0_CMI0    irqTimer1
#define BSP_VECTOR_CMTU1_CMI3    irqTimer2
   
//
/////////////////////////

/////////////////////////
// Prototype for the above Vector funcitons

#ifdef __cplusplus
extern "C" {
#endif

// My app
extern void irqTimer1 (void) __attribute__ ((interrupt));
extern void irqTimer2 (void) __attribute__ ((interrupt));


#ifdef __cplusplus
}
#endif
//
/////////////////////////


// My BSP
#include "Bsp/Renesas/Rx/u62n/Yrdkr62n/Gnurx/Api.h"


//// strapi mapping
//#include "Cpl/Text/_mappings/mingw/strapi.h"


#endif
