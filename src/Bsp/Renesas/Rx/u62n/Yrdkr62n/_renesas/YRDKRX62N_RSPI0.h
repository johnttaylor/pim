/*-------------------------------------------------------------------------*
 * File:  YRDKRX62N_RSPI0.h
 *-------------------------------------------------------------------------*
 * Description:
 *    Y Renesas Development Kit (RDK) for Renesas RX62N Group CPU
 * RSPI Transmitter 0 Operation channel 3.  Routines for RSPI code running
 * on RX62N CPU, RSPI0.  This program uses RSPI0.  The Chip Select for the
 * flash device (LCD) is set to PC_2.  This is the RSPI channel used to
 * communicate with the ST7579 Graphics Display.
 *
 * The RSPI configuration has the baud rate generator set
 * for maximum speed, which is PLCK/2 or 25 MHz.
 *
 * Manufacturer: Renesas
 * Communications Protocol: SPI 3-Wire Transmitt only MASTER SPI
 * Transmitter Number: 0
 * channel: 3
 * Chip Select Port: PC.2
 * SPI Clock Speed: 25MHz
 * Slave Device: OKAYA LCD with ST7579 microprocessor
 *
 *-------------------------------------------------------------------------*/
 
#ifndef __GLYPH__YRDK_RX62N_SPI_0_HEADER_FILE
#define __GLYPH__YRDK_RX62N_SPI_0_HEADER_FILE

/******************************************************************************
Includes “Glyph Include”
******************************************************************************/
#include  "Bsp/Renesas/Rx/_glyph/Glyph.h"

 
/******************************************************************************
Prototypes for the Glyph Communications API
******************************************************************************/
void YRDKRX62N_CommandSend(int8_t cCommand) ;
void YRDKRX62N_DataSend(int8_t cData) ;
T_glyphError YRDKRX62N_RSPIOpen(T_glyphHandle aHandle);

#endif /* __GLYPH__YRDK_RX62N_SPI_0_HEADER_FILE */
 
 
