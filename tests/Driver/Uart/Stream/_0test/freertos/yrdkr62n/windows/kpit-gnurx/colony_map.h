#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

/////////////////////////
// CPL Stuff....

#include "Cpl/System/FreeRTOS/mappings_.h"

// strapi mapping
#include "Cpl/Text/_mappings/_kpitrx/strapi.h"




/////////////////////////
// Define/Set which vectors I am using

// FreeRTOS kernel vectors
#define BSP_VECTOR_ICU_SWINT    vSoftwareInterruptISR
#define BSP_VECTOR_CMTU0_CMI0   vTickISR
   
// Serial Port vectors
#define BSP_VECTOR_SCI2_TXI2    consoleTxISR
#define BSP_VECTOR_SCI2_RXI2    consoleRxISR
#define BSP_VECTOR_SCI2_ERI2    consoleRxISR

//
/////////////////////////

/////////////////////////
// Prototype for the above Vector funcitons

#ifdef __cplusplus
extern "C" {
#endif

// FreeRTOS
extern void vTickISR( void ); 
extern void vSoftwareInterruptISR( void ); 

extern void consoleTxISR( void ) __attribute__ ((interrupt));
extern void consoleRxISR( void ) __attribute__ ((interrupt));


#ifdef __cplusplus
}
#endif
//
/////////////////////////

// My BSP
#include "Bsp/Renesas/Rx/u62n/Yrdkr62n/Gnurx/Api.h"



#endif
