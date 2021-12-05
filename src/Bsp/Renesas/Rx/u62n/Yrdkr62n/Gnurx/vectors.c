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

#include "vectors_.h"

//////////////////////////////////////////////////////////////////////////////

#ifndef USE_BSP_VECTOR_NOT_USED_APP_SUPPLIED
void BSP_VECTOR_NOT_USED(void)
    {
    __asm("nop");  // Something you can set a break point at
    }
#endif

#ifndef USE_BSP_VECTOR_ERROR_TRAP_APP_SUPPLIED
void BSP_VECTOR_ERROR_TRAP(void)
    {
    __asm("nop");  // Something you can set a break point at
    }
#endif

typedef void (*fp) (void);

// Start C++
#ifdef __cplusplus
extern "C" {    
#endif

extern const fp HardwareVectors[];
extern const fp RelocatableVectors[];

// End C++
#ifdef __cplusplus
};
#endif

///////////////////////////////////////////

#define FVECT_SECT    __attribute__ ((section (".fvectors")))


const fp HardwareVectors[] FVECT_SECT  = {
    (fp)0,                                      //;0xffffff80  Reserved
    (fp)0,                                      //;0xffffff84  Reserved
    (fp)0,                                      //;0xffffff88  Reserved
    (fp)0,                                      //;0xffffff8C  Reserved
    (fp)0,                                      //;0xffffff90  Reserved
    (fp)0,                                      //;0xffffff94  Reserved
    (fp)0,                                      //;0xffffff98  Reserved
    (fp)0,                                      //;0xffffff9C  Reserved
    (fp)0,                                      //;0xffffffA0  Reserved
    (fp)0,                                      //;0xffffffA4  Reserved
    (fp)0,                                      //;0xffffffA8  Reserved
    (fp)0,                                      //;0xffffffAC  Reserved
    (fp)0,                                      //;0xffffffB0  Reserved
    (fp)0,                                      //;0xffffffB4  Reserved
    (fp)0,                                      //;0xffffffB8  Reserved
    (fp)0,                                      //;0xffffffBC  Reserved
    (fp)0,                                      //;0xffffffC0  Reserved
    (fp)0,                                      //;0xffffffC4  Reserved
    (fp)0,                                      //;0xffffffC8  Reserved
    (fp)0,                                      //;0xffffffCC  Reserved
    BSP_VECTOR_EXCEP_SUPERVISOR_INSTRUCTION,    //;0xffffffd0  Exception(Supervisor Instruction)
    (fp)0,                                      //;0xffffffd4  Reserved
    (fp)0,                                      //;0xffffffd8  Reserved
    BSP_VECTOR_EXCEP_UNDEFINED_INSTRUCTION,     //;0xffffffdc  Exception(Undefined Instruction)
    (fp)0,                                      //;0xffffffe0  Reserved
    BSP_VECTOR_EXCEP_FLOATING_POINT,            //;0xffffffe4  Exception(Floating Point)
    (fp)0,                                      //;0xffffffe8  Reserved
    (fp)0,                                      //;0xffffffec  Reserved
    (fp)0,                                      //;0xfffffff0  Reserved
    (fp)0,                                      //;0xfffffff4  Reserved
    BSP_VECTOR_NON_MASKABLE_IRQ,                //;0xfffffff8  NMI
    PowerON_Reset                               //;0xfffffffc  RESET                                                                               
};                                                  




///////////////////////////////////////////
#define RVECT_SECT          __attribute__ ((section (".rvectors")))

const fp RelocatableVectors[] RVECT_SECT  = {
    (fp)0,                                            //;0x0000  Reserved
    (fp)0,                                            //;0x0004  Reserved
    (fp)0,                                            //;0x0008  Reserved
    (fp)0,                                            //;0x000C  Reserved
    (fp)0,                                            //;0x0010  Reserved
    (fp)0,                                            //;0x0014  Reserved
    (fp)0,                                            //;0x0018  Reserved
    (fp)0,                                            //;0x001C  Reserved
    (fp)0,                                            //;0x0020  Reserved
    (fp)0,                                            //;0x0024  Reserved
    (fp)0,                                            //;0x0028  Reserved
    (fp)0,                                            //;0x002C  Reserved
    (fp)0,                                            //;0x0030  Reserved
    (fp)0,                                            //;0x0034  Reserved
    (fp)0,                                            //;0x0038  Reserved
    (fp)0,                                            //;0x003C  Reserved
    (fp)BSP_VECTOR_BUSERR,                            //;0x0040  BUSERR
    (fp)0,                                            //;0x0044  Reserved
    (fp)0,                                            //;0x0048  Reserved
    (fp)0,                                            //;0x004C  Reserved
    (fp)0,                                            //;0x0050  Reserved
    (fp)BSP_VECTOR_FCU_FCUERR,                        //;0x0054  FCUERR
    (fp)0,                                            //;0x0058  Reserved
    (fp)BSP_VECTOR_FCU_FRDYI,                         //;0x005C  FRDYI
    (fp)0,                                            //;0x0060  Reserved
    (fp)0,                                            //;0x0064  Reserved
    (fp)0,                                            //;0x0068  Reserved
    (fp)BSP_VECTOR_ICU_SWINT,                         //;0x006C  ICU_SWINT	
    (fp)BSP_VECTOR_CMTU0_CMI0,                        //;0x0070  CMTU0_CMI0						
    (fp)BSP_VECTOR_CMTU0_CMI1,                        //;0x0074  CMTU0_CMI1					
    (fp)BSP_VECTOR_CMTU1_CMI2,                        //;0x0078  CMTU1_CMI2
    (fp)BSP_VECTOR_CMTU1_CMI3,                        //;0x007C  CMTU1_CMI3
    (fp)BSP_VECTOR_ETHER_EINT,                        //;0x0080  ETHER_EINT				
    (fp)0,                                            //;0x0084  Reserved
    (fp)0,                                            //;0x0088  Reserved
    (fp)0,                                            //;0x008C  Reserved
    (fp)BSP_VECTOR_USB0_D0FIFO0,                      //;0x0090  USB0_D0FIFO0				
    (fp)BSP_VECTOR_USB0_D1FIFO0,                      //;0x0094  USB0_D1FIFO0
    (fp)BSP_VECTOR_USB0_USBI0,                        //;0x0098  USB0_USBI0
    (fp)0,                                            //;0x009C  Reserved
    (fp)BSP_VECTOR_USB1_D0FIFO1,                      //;0x00A0  USB1_D0FIFO1
    (fp)BSP_VECTOR_USB1_D1FIFO1,                      //;0x00A4  USB1_D1FIFO1
    (fp)BSP_VECTOR_USB1_USBI1,                        //;0x00A8  USB1_USBI1
    (fp)0,                                            //;0x00AC  Reserved
    (fp)BSP_VECTOR_RSPI0_SPEIO,                       //;0x00B0  RSPI0_SPEIO
    (fp)BSP_VECTOR_RSPI0_SPRI0,                       //;0x00B4  RSPI0_SPRI0
    (fp)BSP_VECTOR_RSPI0_SPTI0,                       //;0x00B8  RSPI0_SPTI0
    (fp)BSP_VECTOR_RSPI0_SPII0,                       //;0x00BC  RSPI0_SPII0
    (fp)BSP_VECTOR_RSPI1_SPEI1,                       //;0x00C0  RSPI1_SPEI1
    (fp)BSP_VECTOR_RSPI1_SPRI1,                       //;0x00C4  RSPI1_SPRI1
    (fp)BSP_VECTOR_RSPI1_SPTI1,                       //;0x00C8  RSPI1_SPTI1
    (fp)BSP_VECTOR_RSPI1_SPII1,                       //;0x00CC  RSPI1_SPII1
    (fp)0,                                            //;0x00D0  Reserved
    (fp)0,                                            //;0x00D4  Reserved
    (fp)0,                                            //;0x00D8  Reserved
    (fp)0,                                            //;0x00DC  Reserved
    (fp)BSP_VECTOR_CAN0_ERS0,                         //;0x00E0  CAN0_ERS0
    (fp)BSP_VECTOR_CAN0_RXF0,                         //;0x00E4  CAN0_RXF0
    (fp)BSP_VECTOR_CAN0_TXF0,                         //;0x00E8  CAN0_TXF0
    (fp)BSP_VECTOR_CAN0_RXM0,                         //;0x00EC  CAN0_RXM0
    (fp)BSP_VECTOR_CAN0_TXM0,                         //;0x00F0  CAN0_TXM0
    (fp)0,                                            //;0x00F4  Reserved
    (fp)BSP_VECTOR_RTC_PRD,                           //;0x00F8  RTC_PRD
    (fp)BSP_VECTOR_RTC_CUP,                           //;0x00FC  RTC_CUP
    (fp)BSP_VECTOR_IRQ0,                              //;0x0100  IRQ0
    (fp)BSP_VECTOR_IRQ1,                              //;0x0104  IRQ1
    (fp)BSP_VECTOR_IRQ2,                              //;0x0108  IRQ2
    (fp)BSP_VECTOR_IRQ3,                              //;0x010C  IRQ3
    (fp)BSP_VECTOR_IRQ4,                              //;0x0110  IRQ4
    (fp)BSP_VECTOR_IRQ5,                              //;0x0114  IRQ5
    (fp)BSP_VECTOR_IRQ6,                              //;0x0118  IRQ6
    (fp)BSP_VECTOR_IRQ7,                              //;0x011C  IRQ7
    (fp)BSP_VECTOR_IRQ8,                              //;0x0120  IRQ8
    (fp)BSP_VECTOR_IRQ9,                              //;0x0124  IRQ9
    (fp)BSP_VECTOR_IRQ10,                             //;0x0128  IRQ10
    (fp)BSP_VECTOR_IRQ11,                             //;0x012C  IRQ11
    (fp)BSP_VECTOR_IRQ12,                             //;0x0130  IRQ12
    (fp)BSP_VECTOR_IRQ13,                             //;0x0134  IRQ13
    (fp)BSP_VECTOR_IRQ14,                             //;0x0138  IRQ14
    (fp)BSP_VECTOR_IRQ15,                             //;0x013C  IRQ15
    (fp)0,                                            //;0x0140  Reserved
    (fp)0,                                            //;0x0144  Reserved
    (fp)0,                                            //;0x0148  Reserved
    (fp)0,                                            //;0x014C  Reserved
    (fp)0,                                            //;0x0150  Reserved
    (fp)0,                                            //;0x0154  Reserved
    (fp)0,                                            //;0x0158  Reserved
    (fp)0,                                            //;0x015C  Reserved
    (fp)0,                                            //;0x0160  Reserved
    (fp)0,                                            //;0x0164  Reserved
    (fp)BSP_VECTOR_USBR_USBR0,                        //;0x0168  USBR_USBR0
    (fp)BSP_VECTOR_USBR_USBR1,                        //;0x016C  USBR_USBR1
    (fp)BSP_VECTOR_RTC_ALM,                           //;0x0170  RTC_ALM
    (fp)0,                                            //;0x0174  Reserved
    (fp)0,                                            //;0x0178  Reserved
    (fp)0,                                            //;0x017C  Reserved
    (fp)BSP_VECTOR_WDT_WOVI,                          //;0x0180  WDT_WOVI
    (fp)0,                                            //;0x0184  Reserved
    (fp)BSP_VECTOR_AD0_ADI0,                          //;0x0188  AD0_ADI0
    (fp)BSP_VECTOR_AD1_ADI1,                          //;0x018C  AD1_ADI1
    (fp)0,                                            //;0x0190  Reserved
    (fp)0,                                            //;0x0194  Reserved
    (fp)BSP_VECTOR_S12AD_ADI12_0,                     //;0x0198  S12AD_ADI12_0
    (fp)0,                                            //;0x019C  Reserved
    (fp)0,                                            //;0x01A0  Reserved
    (fp)0,                                            //;0x01A4  Reserved
    (fp)0,                                            //;0x01A8  Reserved
    (fp)0,                                            //;0x01AC  Reserved
    (fp)0,                                            //;0x01B0  Reserved
    (fp)0,                                            //;0x01B4  Reserved
    (fp)0,                                            //;0x01B8  Reserved
    (fp)0,                                            //;0x01BC  Reserved
    (fp)0,                                            //;0x01C0  Reserved
    (fp)0,                                            //;0x01C4  Reserved
    (fp)BSP_VECTOR_MTU0_TGIA0,                        //;0x01C8  MTU0_TGIA0
    (fp)BSP_VECTOR_MTU0_TGIB0,                        //;0x01CC  MTU0_TGIB0
    (fp)BSP_VECTOR_MTU0_TGICO,                        //;0x01D0  MTU0_TGICO
    (fp)BSP_VECTOR_MTU0_TGID0,                        //;0x01D4  MTU0_TGID0
    (fp)BSP_VECTOR_MTU0_TCIV0,                        //;0x01D8  MTU0_TCIV0
    (fp)BSP_VECTOR_MTU0_TGIE0,                        //;0x01DC  MTU0_TGIE0
    (fp)BSP_VECTOR_MTU0_TGIF0,                        //;0x01E0  MTU0_TGIF0
    (fp)BSP_VECTOR_MTU1_TGIA1,                        //;0x01E4  MTU1_TGIA1				
    (fp)BSP_VECTOR_MTU1_TGIB1,                        //;0x01E8  MTU1_TGIB1
    (fp)BSP_VECTOR_MTU1_TCIV1,                        //;0x01EC  MTU1_TCIV1
    (fp)BSP_VECTOR_MTU1_TCIU1,                        //;0x01F0  MTU1_TCIU1
    (fp)BSP_VECTOR_MTU2_TGIA2,                        //;0x01F4  MTU2_TGIA2
    (fp)BSP_VECTOR_MTU2_TGIB2,                        //;0x01F8  MTU2_TGIB2
    (fp)BSP_VECTOR_MTU2_TCIV2,                        //;0x01FC  MTU2_TCIV2
    (fp)BSP_VECTOR_MTU2_TCIU2,                        //;0x0200  MTU2_TCIU2
    (fp)BSP_VECTOR_MTU3_TGIA3,                        //;0x0204  MTU3_TGIA3
    (fp)BSP_VECTOR_MTU3_TGIB3,                        //;0x0208  MTU3_TGIB3
    (fp)BSP_VECTOR_MTU3_TGIC3,                        //;0x020C  MTU3_TGIC3
    (fp)BSP_VECTOR_MTU3_TGID3,                        //;0x0210  MTU3_TGID3
    (fp)BSP_VECTOR_MTU3_TCIV3,                        //;0x0214  MTU3_TCIV3
    (fp)BSP_VECTOR_MTU4_TGIA4,                        //;0x0218  MTU4_TGIA4
    (fp)BSP_VECTOR_MTU4_TGIB4,                        //;0x021C  MTU4_TGIB4
    (fp)BSP_VECTOR_MTU4_TGIC4,                        //;0x0220  MTU4_TGIC4
    (fp)BSP_VECTOR_MTU4_TGID4,                        //;0x0224  MTU4_TGID4
    (fp)BSP_VECTOR_MTU4_TCIV4,                        //;0x0228  MTU4_TCIV4
    (fp)BSP_VECTOR_MTU5_TGIU5,                        //;0x022C  MTU5_TGIU5
    (fp)BSP_VECTOR_MTU5_TGIV5,                        //;0x0230  MTU5_TGIV5
    (fp)BSP_VECTOR_MTU5_TGIW5,                        //;0x0234  MTU5_TGIW5
    (fp)BSP_VECTOR_MTU6_TGIA6,                        //;0x0238  MTU6_TGIA6
    (fp)BSP_VECTOR_MTU6_TGIB6,                        //;0x023C  MTU6_TGIB6
    (fp)BSP_VECTOR_MTU6_TGIC6,                        //;0x0240  MTU6_TGIC6
    (fp)BSP_VECTOR_MTU6_TGID6,                        //;0x0244  MTU6_TGID6
    (fp)BSP_VECTOR_MTU6_TCIV6,                        //;0x0248  MTU6_TCIV6
    (fp)BSP_VECTOR_MTU6_TGIE6,                        //;0x024C  MTU6_TGIE6
    (fp)BSP_VECTOR_MTU6_TGIF6,                        //;0x0250  MTU6_TGIF6
    (fp)BSP_VECTOR_MTU7_TGIA7,                        //;0x0254  MTU7_TGIA7
    (fp)BSP_VECTOR_MTU7_TGIB7,                        //;0x0258  MTU7_TGIB7
    (fp)BSP_VECTOR_MTU7_TCIV7,                        //;0x025C  MTU7_TCIV7
    (fp)BSP_VECTOR_MTU7_TCIU7,                        //;0x0260  MTU7_TCIU7
    (fp)BSP_VECTOR_MTU8_TGIA8,                        //;0x0264  MTU8_TGIA8
    (fp)BSP_VECTOR_MTU8_TGIB8,                        //;0x0268  MTU8_TGIB8
    (fp)BSP_VECTOR_MTU8_TCIV8,                        //;0x026C  MTU8_TCIV8
    (fp)BSP_VECTOR_MTU8_TCIU8,                        //;0x0270  MTU8_TCIU8
    (fp)BSP_VECTOR_MTU9_TGIA9,                        //;0x0274  MTU9_TGIA9
    (fp)BSP_VECTOR_MTU9_TGIB9,                        //;0x0278  MTU9_TGIB9
    (fp)BSP_VECTOR_MTU9_TGIC9,                        //;0x027C  MTU9_TGIC9
    (fp)BSP_VECTOR_MTU9_TGID9,                        //;0x0280  MTU9_TGID9
    (fp)BSP_VECTOR_MTU9_TCIV9,                        //;0x0284  MTU9_TCIV9
    (fp)BSP_VECTOR_MTU10_TGIA10,                      //;0x0288  MTU10_TGIA10
    (fp)BSP_VECTOR_MTU10_TGIB10,                      //;0x028C  MTU10_TGIB10
    (fp)BSP_VECTOR_MTU10_TGIC10,                      //;0x0290  MTU10_TGIC10
    (fp)BSP_VECTOR_MTU10_TGID10,                      //;0x0294  MTU10_TGID10
    (fp)BSP_VECTOR_MTU10_TCIV10,                      //;0x0298  MTU10_TCIV10
    (fp)BSP_VECTOR_MTU11_TGIU11,                      //;0x029C  MTU11_TGIU11
    (fp)BSP_VECTOR_MTU11_TGIV11,                      //;0x02A0  MTU11_TGIV11
    (fp)BSP_VECTOR_MTU11_TGIW11,                      //;0x02A4  MTU11_TGIW11
    (fp)BSP_VECTOR_POE_OEI1 ,                         //;0x02A8  POE_OEI1
    (fp)BSP_VECTOR_POE_OEI2,                          //;0x02AC  POE_OEI2
    (fp)BSP_VECTOR_POE_OEI3,                          //;0x02B0  POE_OEI3
    (fp)BSP_VECTOR_POE_OEI4,                          //;0x02B4  POE_OEI4
    (fp)BSP_VECTOR_TMR0_CMIA0,                        //;0x02B8  TMR0_CMI0A
    (fp)BSP_VECTOR_TMR0_CMIB0,                        //;0x02BC  TMR0_CMI0B
    (fp)BSP_VECTOR_TMR0_OVI0,                         //;0x02C0  TMR0_OV0I
    (fp)BSP_VECTOR_TMR1_CMIA1,                        //;0x02C4  TMR1_CMI1A
    (fp)BSP_VECTOR_TMR1_CMIB1,                        //;0x02C8  TMR1_CMI1B
    (fp)BSP_VECTOR_TMR1_OVI1,                         //;0x02CC  TMR1_OV1I
    (fp)BSP_VECTOR_TMR2_CMIA2,                        //;0x02D0  TMR2_CMI2A
    (fp)BSP_VECTOR_TMR2_CMIB2,                        //;0x02D4  TMR2_CMI2B
    (fp)BSP_VECTOR_TMR2_OVI2,                         //;0x02D8  TMR2_OV2I
    (fp)BSP_VECTOR_TMR3_CMIA3,                        //;0x02DC  TMR3_CMI3A
    (fp)BSP_VECTOR_TMR3_CMIB3,                        //;0x02E0  TMR3_CMI3B
    (fp)BSP_VECTOR_TMR3_OVI3,                         //;0x02E4  TMR3_OV3I
    (fp)0,                                            //;0x02E8  Reserved
    (fp)0,                                            //;0x02EC  Reserved
    (fp)0,                                            //;0x02F0  Reserved
    (fp)0,                                            //;0x02F4  Reserved
    (fp)0,                                            //;0x02F8  Reserved
    (fp)0,                                            //;0x02FC  Reserved
    (fp)0,                                            //;0x0300  Reserved
    (fp)0,                                            //;0x0304  Reserved
    (fp)0,                                            //;0x0308  Reserved
    (fp)0,                                            //;0x030C  Reserved
    (fp)0,                                            //;0x0310  Reserved
    (fp)0,                                            //;0x0314  Reserved
    (fp)BSP_VECTOR_DMACA_DMAC0I,                      //;0x0318  DMACA_DMAC0I
    (fp)BSP_VECTOR_DMACA_DMAC1I,                      //;0x031C  DMACA_DMAC1I
    (fp)BSP_VECTOR_DMACA_DMAC2I,                      //;0x0320  DMACA_DMAC2I
    (fp)BSP_VECTOR_DMACA_DMAC3I,                      //;0x0324  DMACA_DMAC3I	
    (fp)BSP_VECTOR_EXDMAC_EXDMAC_0I,                  //;0x0328  EXDMAC_EXDMAC_0I
    (fp)BSP_VECTOR_EXDMAC_EXDMAC_1I,                  //;0x032C  EXDMAC_EXDMAC_1I
    (fp)0,                                            //;0x0330  Reserved
    (fp)0,                                            //;0x0334  Reserved
    (fp)0,                                            //;0x0338  Reserved
    (fp)0,                                            //;0x033C  Reserved
    (fp)0,                                            //;0x0340  Reserved
    (fp)0,                                            //;0x0344  Reserved
    (fp)0,                                            //;0x0348  Reserved
    (fp)0,                                            //;0x034C  Reserved
    (fp)0,                                            //;0x0350  Reserved
    (fp)0,                                            //;0x0354  Reserved
    (fp)BSP_VECTOR_SCI0_ERI0,                         //;0x0358  SCI0_ERI0
    (fp)BSP_VECTOR_SCI0_RXI0,                         //;0x035C  SCI0_RXI0
    (fp)BSP_VECTOR_SCI0_TXI0,                         //;0x0360  SCI0_TXI0
    (fp)BSP_VECTOR_SCI0_TEI0,                         //;0x0364  SCI0_TEI0
    (fp)BSP_VECTOR_SCI1_ERI1,                         //;0x0368  SCI1_ERI1
    (fp)BSP_VECTOR_SCI1_RXI1,                         //;0x036C  SCI1_RXI1
    (fp)BSP_VECTOR_SCI1_TXI1,                         //;0x0370  SCI1_TXI1
    (fp)BSP_VECTOR_SCI1_TEI1,                         //;0x0374  SCI1_TEI1
    (fp)BSP_VECTOR_SCI2_ERI2,                         //;0x0378  SCI2_ERI2
    (fp)BSP_VECTOR_SCI2_RXI2,                         //;0x037C  SCI2_RXI2
    (fp)BSP_VECTOR_SCI2_TXI2,                         //;0x0380  SCI2_TXI2
    (fp)BSP_VECTOR_SCI2_TEI2,                         //;0x0384  SCI2_TEI2
    (fp)BSP_VECTOR_SCI3_ERI3,                         //;0x0388  SCI3_ERI3
    (fp)BSP_VECTOR_SCI3_RXI3,                         //;0x038C  SCI3_RXI3
    (fp)BSP_VECTOR_SCI3_TXI3,                         //;0x0390  SCI3_TXI3
    (fp)BSP_VECTOR_SCI3_TEI3,                         //;0x0394  SCI3_TEI3
    (fp)0,                                            //;0x0398  Reserved						
    (fp)0,                                            //;0x039C  Reserved
    (fp)0,                                            //;0x03A0  Reserved
    (fp)0,                                            //;0x03A4  Reserved
    (fp)BSP_VECTOR_SCI5_ERI5,                         //;0x03A8  SCI5_ERI5
    (fp)BSP_VECTOR_SCI5_RXI5,                         //;0x03AC  SCI5_RXI5
    (fp)BSP_VECTOR_SCI5_TXI5,                         //;0x03B0  SCI5_TXI5
    (fp)BSP_VECTOR_SCI5_TEI5,                         //;0x03B4  SCI5_TEI5
    (fp)BSP_VECTOR_SCI6_ERI6,                         //;0x03B8  SCI6_ERI6
    (fp)BSP_VECTOR_SCI6_RXI6,                         //;0x03BC  SCI6_RXI6
    (fp)BSP_VECTOR_SCI6_TXI6,                         //;0x03C0  SCI6_TXI6
    (fp)BSP_VECTOR_SCI6_TEI6,                         //;0x03C4  SCI6_TEI6
    (fp)0,                                            //;0x03C8  Reserved
    (fp)0,                                            //;0x03CC  Reserved
    (fp)0,                                            //;0x03D0  Reserved
    (fp)0,                                            //;0x03D4  Reserved
    (fp)BSP_VECTOR_RIIC0_ICEEI0,                      //;0x03D8  RIIC0_ICEEI0
    (fp)BSP_VECTOR_RIIC0_ICRXI0,                      //;0x03DC  RIIC0_ICRXI0
    (fp)BSP_VECTOR_RIIC0_ICTXI0,                      //;0x03E0  RIIC0_ICTXI0
    (fp)BSP_VECTOR_RIIC0_ICTEI0,                      //;0x03E4  RIIC0_ICTEI0
    (fp)BSP_VECTOR_RIIC1_ICEEI1,                      //;0x03E8  RIIC1_ICEEI1
    (fp)BSP_VECTOR_RIIC1_ICRXI1,                      //;0x03EC  RIIC1_ICRXI1
    (fp)BSP_VECTOR_RIIC1_ICTXI1,                      //;0x03F0  RIIC1_ICTXI1
    (fp)BSP_VECTOR_RIIC1_ICTEI1,                      //;0x03F4  RIIC1_ICTEI1
    (fp)0,                                            //;0x03F8  Reserved
    (fp)0                                             //;0x03FC  Reserved
};

