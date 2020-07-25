#ifndef Bsp_YRDKR62N_irq_handlers_h_
#define Bsp_YRDKR62N_irq_handlers_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file 


    This file provides/defines the functions for the CPU's vector/interrupt
    handlers.  The functions ALL have a default (empty irq funciton) except 
    for the reset/power-on vector which the the BSP provides.  The application 
    can 'hook-in' there our IRQ handlers placing the following code in their 
    'colony_map.h' header file BEFORE the #include to this BSP's "Api.h" header 
    file:

    
        extern void myIrqHandler (void) __attribute__ ((interrupt));
        #define BSP_EXCEP_SUPERVISOR_INSTRUCTION    myIrqHandler

    
    NOTE: The 'not-used' and 'error-trap' vector functions can be supplied by 
          the Application be enabling/using the following 'config' switche via 
          the 'colony_config.h" header file.

            USE_BSP_VECTOR_NOT_USED_APP_SUPPLIED
            USE_BSP_VECTOR_ERROR_TRAP_APP_SUPPLIED


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h - and then configure your project's 
    'colony_map.h' to include THIS file.


    HOW TO COMPILE AND LINK 'vectors.cpp'
    ------------------------------------
    Because there are no references to the vector table if 'vectors.cpp' is
    compiled and then put into a library - which then the application links
    against -->the vector table will be MISSING. The link-against-library
    is the behavior of the Colony.Core's default build engine: NQBP.  So
    there are two ways around this:
    
        1) Use the 'firstobjs' mechanism in NQBP to force NQBP to link
           the vectors.o file directly.
        2) Cheat some how.  In the 'start.asm' file there is unreachable
           code that references the fixed vector table.  Since the PowerOn_Reset()
           function is explicity called out in the link process - it gets pick-up
           even if doing the link-by-library things.  And by have it reference
           the vector table - it make the link-by-library think work - at the
           cost of one 'bsr.a' instruction.
*/


//////////////////////////////////////////////////////////////////////////////

#include "colony_config.h"
#include "colony_map.h"


// Start C++
#ifdef __cplusplus
extern "C" {
#endif

/// Declare the reset/poweron vector
extern void PowerON_Reset(void) __attribute__ ((interrupt));  	

/// Declare the 'not-used' vector
extern void BSP_VECTOR_NOT_USED(void)  __attribute__ ((interrupt));

/// Declare the 'error-trap' vector
extern void BSP_VECTOR_ERROR_TRAP(void)  __attribute__ ((interrupt));

// END C++
#ifdef __cplusplus
};
#endif


//////////////////////////////////////////////////////////////////////////////
/*
** FIXED Vector/IRQ Functions
*/

#ifndef BSP_VECTOR_EXCEP_SUPERVISOR_INSTRUCTION
#define BSP_VECTOR_EXCEP_SUPERVISOR_INSTRUCTION         BSP_VECTOR_ERROR_TRAP
#endif
#ifndef BSP_VECTOR_EXCEP_UNDEFINED_INSTRUCTION
#define BSP_VECTOR_EXCEP_UNDEFINED_INSTRUCTION          BSP_VECTOR_ERROR_TRAP
#endif
#ifndef BSP_VECTOR_EXCEP_FLOATING_POINT
#define BSP_VECTOR_EXCEP_FLOATING_POINT                 BSP_VECTOR_ERROR_TRAP
#endif
#ifndef BSP_VECTOR_NON_MASKABLE_IRQ
#define BSP_VECTOR_NON_MASKABLE_IRQ                     BSP_VECTOR_ERROR_TRAP
#endif


//////////////////////////////////////////////////////////////////////////////
/*
** RELOCATEABLE Vector/IRQ Functions
*/
#ifndef BSP_VECTOR_BUSERR
#define BSP_VECTOR_BUSERR                               BSP_VECTOR_ERROR_TRAP
#endif
#ifndef BSP_VECTOR_FCU_FCUERR
#define BSP_VECTOR_FCU_FCUERR                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_FCU_FRDYI
#define BSP_VECTOR_FCU_FRDYI                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_ICU_SWINT
#define BSP_VECTOR_ICU_SWINT				            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CMTU0_CMI0
#define BSP_VECTOR_CMTU0_CMI0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CMTU0_CMI1
#define BSP_VECTOR_CMTU0_CMI1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CMTU1_CMI2
#define BSP_VECTOR_CMTU1_CMI2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CMTU1_CMI3
#define BSP_VECTOR_CMTU1_CMI3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_ETHER_EINT
#define BSP_VECTOR_ETHER_EINT	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USB0_D0FIFO0
#define BSP_VECTOR_USB0_D0FIFO0                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USB0_D1FIFO0
#define BSP_VECTOR_USB0_D1FIFO0                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USB0_USBI0
#define BSP_VECTOR_USB0_USBI0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USB1_D0FIFO1
#define BSP_VECTOR_USB1_D0FIFO1	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USB1_D1FIFO1
#define BSP_VECTOR_USB1_D1FIFO1                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USB1_USBI1
#define BSP_VECTOR_USB1_USBI1	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI0_SPEIO
#define BSP_VECTOR_RSPI0_SPEIO                          BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI0_SPRI0
#define BSP_VECTOR_RSPI0_SPRI0                          BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI0_SPTI0
#define BSP_VECTOR_RSPI0_SPTI0                          BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI0_SPII0
#define BSP_VECTOR_RSPI0_SPII0                          BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI1_SPEI1
#define BSP_VECTOR_RSPI1_SPEI1	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI1_SPRI1
#define BSP_VECTOR_RSPI1_SPRI1                          BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI1_SPTI1
#define BSP_VECTOR_RSPI1_SPTI1                          BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RSPI1_SPII1
#define BSP_VECTOR_RSPI1_SPII1	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CAN0_ERS0
#define BSP_VECTOR_CAN0_ERS0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CAN0_RXF0
#define BSP_VECTOR_CAN0_RXF0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CAN0_TXF0
#define BSP_VECTOR_CAN0_TXF0	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CAN0_RXM0
#define BSP_VECTOR_CAN0_RXM0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_CAN0_TXM0
#define BSP_VECTOR_CAN0_TXM0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RTC_PRD
#define BSP_VECTOR_RTC_PRD                              BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RTC_CUP
#define BSP_VECTOR_RTC_CUP                              BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ0
#define BSP_VECTOR_IRQ0                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ1
#define BSP_VECTOR_IRQ1                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ2
#define BSP_VECTOR_IRQ2                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ3
#define BSP_VECTOR_IRQ3                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ4
#define BSP_VECTOR_IRQ4                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ5
#define BSP_VECTOR_IRQ5                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ6
#define BSP_VECTOR_IRQ6                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ7
#define BSP_VECTOR_IRQ7                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ8
#define BSP_VECTOR_IRQ8                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ9
#define BSP_VECTOR_IRQ9                                 BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ10
#define BSP_VECTOR_IRQ10                                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ11
#define BSP_VECTOR_IRQ11                                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ12
#define BSP_VECTOR_IRQ12                                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ13
#define BSP_VECTOR_IRQ13                                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ14
#define BSP_VECTOR_IRQ14                                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_IRQ15
#define BSP_VECTOR_IRQ15                                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USBR_USBR0
#define BSP_VECTOR_USBR_USBR0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_USBR_USBR1
#define BSP_VECTOR_USBR_USBR1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RTC_ALM
#define BSP_VECTOR_RTC_ALM		                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_WDT_WOVI
#define BSP_VECTOR_WDT_WOVI                             BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_AD0_ADI0
#define BSP_VECTOR_AD0_ADI0                             BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_AD1_ADI1
#define BSP_VECTOR_AD1_ADI1                             BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_S12AD_ADI12_0
#define BSP_VECTOR_S12AD_ADI12_0                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TGIA0
#define BSP_VECTOR_MTU0_TGIA0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TGIB0
#define BSP_VECTOR_MTU0_TGIB0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TGICO
#define BSP_VECTOR_MTU0_TGICO	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TGID0
#define BSP_VECTOR_MTU0_TGID0	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TCIV0
#define BSP_VECTOR_MTU0_TCIV0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TGIE0
#define BSP_VECTOR_MTU0_TGIE0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU0_TGIF0
#define BSP_VECTOR_MTU0_TGIF0                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU1_TGIA1
#define BSP_VECTOR_MTU1_TGIA1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU1_TGIB1
#define BSP_VECTOR_MTU1_TGIB1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU1_TCIV1
#define BSP_VECTOR_MTU1_TCIV1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU1_TCIU1
#define BSP_VECTOR_MTU1_TCIU1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU2_TGIA2
#define BSP_VECTOR_MTU2_TGIA2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU2_TGIB2
#define BSP_VECTOR_MTU2_TGIB2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU2_TCIV2
#define BSP_VECTOR_MTU2_TCIV2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU2_TCIU2
#define BSP_VECTOR_MTU2_TCIU2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU3_TGIA3
#define BSP_VECTOR_MTU3_TGIA3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU3_TGIB3
#define BSP_VECTOR_MTU3_TGIB3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU3_TGIC3
#define BSP_VECTOR_MTU3_TGIC3	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU3_TGID3
#define BSP_VECTOR_MTU3_TGID3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU3_TCIV3
#define BSP_VECTOR_MTU3_TCIV3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU4_TGIA4
#define BSP_VECTOR_MTU4_TGIA4	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU4_TGIB4
#define BSP_VECTOR_MTU4_TGIB4                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU4_TGIC4
#define BSP_VECTOR_MTU4_TGIC4                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU4_TGID4
#define BSP_VECTOR_MTU4_TGID4                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU4_TCIV4
#define BSP_VECTOR_MTU4_TCIV4			                BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU5_TGIU5
#define BSP_VECTOR_MTU5_TGIU5                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU5_TGIV5
#define BSP_VECTOR_MTU5_TGIV5                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU5_TGIW5
#define BSP_VECTOR_MTU5_TGIW5                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TGIA6
#define BSP_VECTOR_MTU6_TGIA6                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TGIB6
#define BSP_VECTOR_MTU6_TGIB6                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TGIC6
#define BSP_VECTOR_MTU6_TGIC6                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TGID6
#define BSP_VECTOR_MTU6_TGID6	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TCIV6
#define BSP_VECTOR_MTU6_TCIV6                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TGIE6
#define BSP_VECTOR_MTU6_TGIE6	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU6_TGIF6
#define BSP_VECTOR_MTU6_TGIF6                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU7_TGIA7
#define BSP_VECTOR_MTU7_TGIA7                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU7_TGIB7
#define BSP_VECTOR_MTU7_TGIB7	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU7_TCIV7
#define BSP_VECTOR_MTU7_TCIV7                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU7_TCIU7
#define BSP_VECTOR_MTU7_TCIU7                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU8_TGIA8
#define BSP_VECTOR_MTU8_TGIA8	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU8_TGIB8
#define BSP_VECTOR_MTU8_TGIB8                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU8_TCIV8
#define BSP_VECTOR_MTU8_TCIV8	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU8_TCIU8
#define BSP_VECTOR_MTU8_TCIU8                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU9_TGIA9
#define BSP_VECTOR_MTU9_TGIA9                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU9_TGIB9
#define BSP_VECTOR_MTU9_TGIB9                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU9_TGIC9
#define BSP_VECTOR_MTU9_TGIC9                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU9_TGID9
#define BSP_VECTOR_MTU9_TGID9	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU9_TCIV9
#define BSP_VECTOR_MTU9_TCIV9                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU10_TGIA10
#define BSP_VECTOR_MTU10_TGIA10                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU10_TGIB10
#define BSP_VECTOR_MTU10_TGIB10                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU10_TGIC10
#define BSP_VECTOR_MTU10_TGIC10                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU10_TGID10
#define BSP_VECTOR_MTU10_TGID10	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU10_TCIV10
#define BSP_VECTOR_MTU10_TCIV10	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU11_TGIU11
#define BSP_VECTOR_MTU11_TGIU11                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU11_TGIV11
#define BSP_VECTOR_MTU11_TGIV11                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_MTU11_TGIW11
#define BSP_VECTOR_MTU11_TGIW11		                    BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_POE_OEI1
#define BSP_VECTOR_POE_OEI1	                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_POE_OEI2
#define BSP_VECTOR_POE_OEI2                             BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_POE_OEI3
#define BSP_VECTOR_POE_OEI3                             BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_POE_OEI4
#define BSP_VECTOR_POE_OEI4                             BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR0_CMIA0
#define BSP_VECTOR_TMR0_CMIA0	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR0_CMIB0
#define BSP_VECTOR_TMR0_CMIB0	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR0_OVI0
#define BSP_VECTOR_TMR0_OVI0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR1_CMIA1
#define BSP_VECTOR_TMR1_CMIA1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR1_CMIB1
#define BSP_VECTOR_TMR1_CMIB1                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR1_OVI1
#define BSP_VECTOR_TMR1_OVI1                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR2_CMIA2
#define BSP_VECTOR_TMR2_CMIA2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR2_CMIB2
#define BSP_VECTOR_TMR2_CMIB2                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR2_OVI2
#define BSP_VECTOR_TMR2_OVI2                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR3_CMIA3
#define BSP_VECTOR_TMR3_CMIA3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR3_CMIB3
#define BSP_VECTOR_TMR3_CMIB3                           BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_TMR3_OVI3
#define BSP_VECTOR_TMR3_OVI3                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_DMACA_DMAC0I
#define BSP_VECTOR_DMACA_DMAC0I                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_DMACA_DMAC1I
#define BSP_VECTOR_DMACA_DMAC1I                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_DMACA_DMAC2I
#define BSP_VECTOR_DMACA_DMAC2I	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_DMACA_DMAC3I
#define BSP_VECTOR_DMACA_DMAC3I                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_EXDMAC_EXDMAC_0I
#define BSP_VECTOR_EXDMAC_EXDMAC_0I                     BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_EXDMAC_EXDMAC_1I
#define BSP_VECTOR_EXDMAC_EXDMAC_1I	                    BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI0_ERI0
#define BSP_VECTOR_SCI0_ERI0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI0_RXI0
#define BSP_VECTOR_SCI0_RXI0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI0_TXI0
#define BSP_VECTOR_SCI0_TXI0	                        BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI0_TEI0
#define BSP_VECTOR_SCI0_TEI0                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI1_ERI1
#define BSP_VECTOR_SCI1_ERI1                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI1_RXI1
#define BSP_VECTOR_SCI1_RXI1                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI1_TXI1
#define BSP_VECTOR_SCI1_TXI1                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI1_TEI1
#define BSP_VECTOR_SCI1_TEI1                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI2_ERI2
#define BSP_VECTOR_SCI2_ERI2		                    BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI2_RXI2
#define BSP_VECTOR_SCI2_RXI2                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI2_TXI2
#define BSP_VECTOR_SCI2_TXI2                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI2_TEI2
#define BSP_VECTOR_SCI2_TEI2                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI3_ERI3
#define BSP_VECTOR_SCI3_ERI3                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI3_RXI3
#define BSP_VECTOR_SCI3_RXI3                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI3_TXI3
#define BSP_VECTOR_SCI3_TXI3                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI3_TEI3
#define BSP_VECTOR_SCI3_TEI3                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI5_ERI5
#define BSP_VECTOR_SCI5_ERI5		                    BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI5_RXI5
#define BSP_VECTOR_SCI5_RXI5                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI5_TXI5
#define BSP_VECTOR_SCI5_TXI5                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI5_TEI5
#define BSP_VECTOR_SCI5_TEI5                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI6_ERI6
#define BSP_VECTOR_SCI6_ERI6                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI6_RXI6
#define BSP_VECTOR_SCI6_RXI6                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI6_TXI6
#define BSP_VECTOR_SCI6_TXI6                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_SCI6_TEI6
#define BSP_VECTOR_SCI6_TEI6                            BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC0_ICEEI0
#define BSP_VECTOR_RIIC0_ICEEI0                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC0_ICRXI0
#define BSP_VECTOR_RIIC0_ICRXI0                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC0_ICTXI0
#define BSP_VECTOR_RIIC0_ICTXI0                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC0_ICTEI0
#define BSP_VECTOR_RIIC0_ICTEI0                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC1_ICEEI1
#define BSP_VECTOR_RIIC1_ICEEI1                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC1_ICRXI1
#define BSP_VECTOR_RIIC1_ICRXI1                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC1_ICTXI1
#define BSP_VECTOR_RIIC1_ICTXI1                         BSP_VECTOR_NOT_USED
#endif
#ifndef BSP_VECTOR_RIIC1_ICTEI1
#define BSP_VECTOR_RIIC1_ICTEI1                         BSP_VECTOR_NOT_USED
#endif


//////////////////////////////////////////////////////////


#endif  // end header latch
