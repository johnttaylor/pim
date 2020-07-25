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



/////////////////////////////////////
uint8_t Bsp_uartInitPin_( uint8_t sciPortID, uint8_t pinSelect )
    {
    // Properly configure RX input pin
    switch( sciPortID )
        {
        // This is an error! 
        default:
            return 0;
            // this is NOT a fall through case!

        case 0:
            break;

        case 1:
            PORT2.ICR.BIT.B0 = 1;
            break;

        case 2:
            IOPORT.PFFSCI.BIT.SCI2S = pinSelect;     // Select A/B pin configuration
            if ( pinSelect )
                {
                PORT5.ICR.BIT.B2 = 1;
                }
            else
                {
                PORT1.ICR.BIT.B2 = 1;
                }
            break;

        case 3:
            PORT2.ICR.BIT.B5 = 1;
            break;


        case 5:
            PORTC.ICR.BIT.B2 = 1;
            break;

        case 6:
            PORT3.ICR.BIT.B3 = 1;
            break;              
        }

    return 1;
    }


