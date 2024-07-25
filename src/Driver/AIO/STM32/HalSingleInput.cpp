/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2023  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Driver/AIO/HalSingleInput.h"
#include "HalSingleInput.h"
#include <stdint.h>

bool driverAIOHalSingleInputStm32_sample( ADC_HandleTypeDef* inputPinToSampleHdl, uint32_t& dstADCBits )
{
    HAL_ADC_Start( inputPinToSampleHdl );
    HAL_StatusTypeDef status = HAL_ADC_PollForConversion( &hadc1, 1 ); // Wait at-most 1msec
    if ( status != HAL_OK )
    {
        return false;
    }

    dstADCBits = HAL_ADC_GetValue( inputPinToSampleHdl );
    return true;
}

uint8_t driverAIOHalSingleInputStm32_setADCSize( ADC_HandleTypeDef* inputToSampleHdl, uint8_t numADCBits )
{
    return OPTION_DRIVEDR_AIO_STM32_SINGLE_INPUT_ADC_RESOLUTION;  // Fixed
}

