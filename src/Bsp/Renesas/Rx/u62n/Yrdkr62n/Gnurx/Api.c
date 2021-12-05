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

#include "Bsp/Api.h"


////////////////////////////////////////////////////////

/// Private: LED Constants
#define  LED4_                  PORTD.DR.BIT.B5
#define  LED5_                  PORTE.DR.BIT.B3
#define  LED6_                  PORTD.DR.BIT.B2
#define  LED7_                  PORTE.DR.BIT.B0
#define  LED8_                  PORTD.DR.BIT.B4
#define  LED9_                  PORTE.DR.BIT.B2
#define  LED10_                 PORTD.DR.BIT.B1
#define  LED11_                 PORTD.DR.BIT.B7
#define  LED12_                 PORTD.DR.BIT.B3
#define  LED13_                 PORTE.DR.BIT.B1
#define  LED14_                 PORTD.DR.BIT.B0
#define  LED15_                 PORTD.DR.BIT.B6

// Private: Push button Switch Constants
#define  PUSH_BUTTON_SW1_       PORT4.PORT.BIT.B0
#define  PUSH_BUTTON_SW2_       PORT4.PORT.BIT.B1
#define  PUSH_BUTTON_SW3_       PORT4.PORT.BIT.B2


///////////////////////////////////////////
void Bsp_Api_initialize( void )
    {
	// Set clock speeds 
    uint32_t sckcr = 0;
	sckcr += (BSP_SYSCLK_ICLK_MUL==8) ? (0ul << 24) : (BSP_SYSCLK_ICLK_MUL==4) ? (1ul << 24) : (BSP_SYSCLK_ICLK_MUL==2) ? (2ul << 24) : (3ul << 24);
	sckcr += (BSP_SYSCLK_BCLK_MUL==8) ? (0ul << 16) : (BSP_SYSCLK_BCLK_MUL==4) ? (1ul << 16) : (BSP_SYSCLK_BCLK_MUL==2) ? (2ul << 16) : (3ul << 16);
	sckcr += (BSP_SYSCLK_PCLK_MUL==8) ? (0ul <<  8) : (BSP_SYSCLK_PCLK_MUL==4) ? (1ul <<  8) : (BSP_SYSCLK_PCLK_MUL==2) ? (2ul <<  8) : (3ul <<  8);
	SYSTEM.SCKCR.LONG = sckcr;



    // Configure push button Switches (SW1, SW2, SW3) for polled input
    PORT4.DDR.BIT.B0 = 0;
    PORT4.DDR.BIT.B1 = 0;
    PORT4.DDR.BIT.B2 = 0;
    PORT4.ICR.BIT.B0 = 1;
    PORT4.ICR.BIT.B1 = 1;
    PORT4.ICR.BIT.B2 = 1;


    // Configure 'Circle' LEDs (set port pins as Outputs)
    PORTD.DDR.BIT.B0 = 1;      
    PORTD.DDR.BIT.B1 = 1;
    PORTD.DDR.BIT.B2 = 1;
    PORTD.DDR.BIT.B3 = 1;
    PORTD.DDR.BIT.B4 = 1;
    PORTD.DDR.BIT.B5 = 1;
    PORTD.DDR.BIT.B6 = 1;
    PORTD.DDR.BIT.B7 = 1;
    PORTE.DDR.BIT.B0 = 1;
    PORTE.DDR.BIT.B1 = 1;
    PORTE.DDR.BIT.B2 = 1;
    PORTE.DDR.BIT.B3 = 1;

    // Default all of the circle LEDs to off
    Bsp_setCircleLED( 0, 0 );                                               
    }




///////////////////////////////////////////
void Bsp_setCircleLED( uint8_t ledIndex, uint8_t setToOn )
    {
    switch( ledIndex ) 
        {
        case 0:
            
            LED4_  = !setToOn;
            LED5_  = !setToOn;
            LED6_  = !setToOn;
            LED7_  = !setToOn;
            LED8_  = !setToOn;
            LED9_  = !setToOn;
            LED10_ = !setToOn;
            LED11_ = !setToOn;
            LED12_ = !setToOn;
            LED13_ = !setToOn;
            LED14_ = !setToOn;
            LED15_ = !setToOn;
            break;

        case 4:
            LED4_ = !setToOn;
            break;

        case 5:
            LED5_ = !setToOn;
            break;

        case 6:
            LED6_ = !setToOn;
            break;

        case 7:
            LED7_ = !setToOn;
            break;

        case 8:
            LED8_ = !setToOn;
            break;

        case 9:
            LED9_ = !setToOn;
            break;

        case 10:
            LED10_ = !setToOn;
            break;

        case 11:
            LED11_ = !setToOn;
            break;

        case 12:
            LED12_ = !setToOn;
            break;

        case 13:
            LED13_ = !setToOn;
            break;

        case 14:
            LED14_ = !setToOn;
            break;

        case 15:
            LED15_ = !setToOn;
            break;

        default:
            break;
        }
    }

void Bsp_toggleCircleLED( uint8_t ledIndex )
    {
    switch( ledIndex ) 
        {
        case 0:
            
            LED4_  ^= 1;
            LED5_  ^= 1;
            LED6_  ^= 1;
            LED7_  ^= 1;
            LED8_  ^= 1;
            LED9_  ^= 1;
            LED10_ ^= 1;
            LED11_ ^= 1;
            LED12_ ^= 1;
            LED13_ ^= 1;
            LED14_ ^= 1;
            LED15_ ^= 1;
            break;

        case 4:
            LED4_ ^= 1;
            break;

        case 5:
            LED5_ ^= 1;
            break;

        case 6:
            LED6_ ^= 1;
            break;

        case 7:
            LED7_ ^= 1;
            break;

        case 8:
            LED8_ ^= 1;
            break;

        case 9:
            LED9_ ^= 1;
            break;

        case 10:
            LED10_ ^= 1;
            break;

        case 11:
            LED11_ ^= 1;
            break;

        case 12:
            LED12_ ^= 1;
            break;

        case 13:
            LED13_ ^= 1;
            break;

        case 14:
            LED14_ ^= 1;
            break;

        case 15:
            LED15_ ^= 1;
            break;

        default:
            break;
        }
    }


///////////////////////////////////////////
uint8_t Bsp_isButtonPressed( uint8_t buttonIndex )
    {
    switch( buttonIndex )
        {
        case 1:
            return !PUSH_BUTTON_SW1_; 
            break;
        
        case 2:
            return !PUSH_BUTTON_SW2_; 
            break;
        
        case 3:
            return !PUSH_BUTTON_SW3_; 
            break;

        default:
            break;
        }
   
    return 0;
    }

