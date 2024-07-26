#ifndef Driver_Aio_HalSingleInput_h_
#define Driver_Aio_HalSingleInput_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2022  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file 
    
    This file defines a hardware abstraction layer (HAL) for sampling a single
    Analog input source.

    NOTE: NO 'Initialization' method is provided/defined in this interface -
          this is intentional! The initialization left to the target's platform
          specific code and/or BSP.  The application is RESPONSIBLE for making 
          sure that the platform specific initialization happens BEFORE any of 
          the drivers are started/used.  
*/


/*-------------- PUBLIC API ------------------------------------------------*/

#include "colony_map.h"


/** This data type defines the platform specific 'handle' to a analog input.
 */
#define DriverAIOHalSingleInput_T                    DriverAIOHalSingleInput_T_MAP



/*-------------- PUBLIC API ------------------------------------------------*/
/** This method samples the ADC channel/signal/pin.  The number of ADC bits
    is returned via the 'dstADCBits' argument. The method returns true when a
    successful reading was taken; else false is returned.

    The number of bits in 'dstADCBits' is platform specific and/or is set
    by the Driver_AIO_HalSingle_setADCSize() method.

    Prototype:
        bool Driver_AIO_HalSingleInput_sample( DriverAIOHalSingleInput_T inputToSampleHdl, uint32_t& dstADCBits );
 */
#define Driver_AIO_HalSingleInput_sample             Driver_AIO_HalSingleInput_sample_MAP


/** Sets the number of bits/resolution of the ADC when sampling when using
    Driver_HalSingle_sample().  The method returns the new/current ADC 
    resolution. NOTE: The actual number of ADC Bits/resolution is platform 
    dependent, i.e may not be able to support the requested resolution.  The
    application is ENCOURRAGED to verify that the method returns the same value
    that was request.

    Prototype:
        uint8_t Driver_AIO_HalSingleInput_setADCSize( DriverAIOHalSingleInput_T inputToSampleHdl, uint8_t numADCBits );
*/
#define Driver_AIO_HalSingle_setADCSize         Driver_AIO_HalSingleInput_setADCSize_MAP


/*--------------------------------------------------------------------------*/
#endif  
