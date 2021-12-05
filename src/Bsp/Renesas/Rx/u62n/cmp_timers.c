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

#include "cmp_timers.h"


////////////////////////////////////////////////////////


///////////////////////////////////////////
uint8_t Bsp_initCompareMatchTimer( uint8_t channelNum, uint32_t irqRateInHZ, uint8_t pclkDivider, uint8_t irqPriority )
    {
    // Validate the IRQ Priority
    if ( irqPriority < 1 || irqPriority > 15 )
        {
        return 0;
        }


    // Convert pclk divider to a numeric value
    uint16_t divider = 0;
    switch( pclkDivider )
        {
        default:
            return 0;
            // this is NOT a fall through case!

        case 0:
            divider = 8;
            break;

        case 1:
            divider = 32;
            break;

        case 2:
            divider = 128;
            break;

        case 3:
            divider = 512;
            break;
        }


    // Calculate the Match value. Note: The IRQ generation is DELAYED 1 count AFTER the match
	uint16_t match = (uint16_t) ((BSP_SYSCLK_PCLK_FREQUENCY / divider / irqRateInHZ) -1);   


    // Configure the Timer Channel
    switch( channelNum )
        {
        // This is an error! 
        default:
            return 0;
            // this is NOT a fall through case!

        case 0:
	        MSTP( CMT0 )       = 0;             // Enable the Channel
	        CMT0.CMCR.BIT.CMIE = 1;             // Enable IRQ on match
	        CMT0.CMCOR         = match;         // Set match value.
	        CMT0.CMCR.BIT.CKS  = 0;             // Set the divider       
	        IPR( CMT0,CMI0 )   = irqPriority;   // Set IRQ priority;
	        IEN( CMT0,CMI0 )   = 1;             // Enable the timer interrupt. Note: Until I start the timer - this will-not/cannnot fire
            break;

        case 1:
	        MSTP( CMT1 )       = 0;            
	        CMT1.CMCR.BIT.CMIE = 1;            
	        CMT1.CMCOR         = match;        
	        CMT1.CMCR.BIT.CKS  = 0;            
	        IPR( CMT1,CMI1 )   = irqPriority;   
	        IEN( CMT1,CMI1 )   = 1;            
            break;

        case 2:
	        MSTP( CMT2 )       = 0;            
	        CMT2.CMCR.BIT.CMIE = 1;            
	        CMT2.CMCOR         = match;        
	        CMT2.CMCR.BIT.CKS  = 0;            
	        IPR( CMT2,CMI2 )   = irqPriority;   
	        IEN( CMT2,CMI2 )   = 1;            
            break;

        case 3:
	        MSTP( CMT3 )       = 0;            
	        CMT3.CMCR.BIT.CMIE = 1;            
	        CMT3.CMCOR         = match;        
	        CMT3.CMCR.BIT.CKS  = 0;            
	        IPR( CMT3,CMI3 )   = irqPriority;   
	        IEN( CMT3,CMI3 )   = 1;            
            break;
        }

    return 1;
    }
     
        
uint8_t Bsp_startCompareMatchTimer( uint8_t channelNum )
    {
    // Configure the Timer Channel
    switch( channelNum )
        {
        // This is an error! 
        default:
            return 0;
            // this is NOT a fall through case!

        case 0:
	        CMT.CMSTR0.BIT.STR0 = 1; 
            break;

        case 1:
	        CMT.CMSTR0.BIT.STR1 = 1; 
            break;

        case 2:
	        CMT.CMSTR1.BIT.STR2 = 1; 
            break;

        case 3:
	        CMT.CMSTR1.BIT.STR3 = 1; 
            break;
        }

    return 1;
    }


uint8_t Bsp_stopCompareMatchTimer( uint8_t channelNum )
    {
    // Configure the Timer Channel
    switch( channelNum )
        {
        // This is an error! 
        default:
            return 0;
            // this is NOT a fall through case!

        case 0:
	        CMT.CMSTR0.BIT.STR0 = 0; // Stop the timer
	        IEN( CMT0,CMI0 )    = 0; // Disable the timer interrupt
	        IR( CMT0,CMI0 )     = 0; // Clear any pending/leftover IR request flag
            break;

        case 1:
	        CMT.CMSTR0.BIT.STR1 = 0; 
	        IEN( CMT1,CMI1 )    = 0; 
	        IR( CMT1,CMI1 )     = 0; 
            break;

        case 2:
	        CMT.CMSTR1.BIT.STR2 = 0; 
	        IEN( CMT2,CMI2 )    = 0; 
	        IR( CMT2,CMI2 )     = 0; 
            break;

        case 3:
	        CMT.CMSTR1.BIT.STR3 = 0; 
	        IEN( CMT3,CMI3 )    = 0; 
	        IR( CMT3,CMI3 )     = 0; 
            break;
        }

    return 1;
    }


