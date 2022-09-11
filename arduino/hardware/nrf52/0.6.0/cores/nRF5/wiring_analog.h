/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * \brief SAMD products have only one reference for ADC
 */
#ifdef NRF52
typedef enum _eAnalogReference
{
  AR_DEFAULT,
  AR_INTERNAL,          // 0.6V Ref * 6 = 0..3.6V
  AR_INTERNAL_3_0,      // 0.6V Ref * 5 = 0..3.0V
  AR_INTERNAL_2_4,      // 0.6V Ref * 4 = 0..2.4V
  AR_INTERNAL_1_8,      // 0.6V Ref * 3 = 0..1.8V
  AR_INTERNAL_1_2,      // 0.6V Ref * 2 = 0..1.6V
  AR_VDD4               // VDD/4 REF * 4 = 0..VDD
} eAnalogReference ;
#else
typedef enum _eAnalogReference
{
  AR_DEFAULT,
  AR_VBG,
  AR_SUPPLY_ONE_HALF,
  AR_SUPPLY_ONE_THIRD,
  AR_EXT0,
  AR_EXT1
} eAnalogReference ;
#endif


/*
 * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
 * This function is kept only for compatibility with existing AVR based API.
 *
 * \param ulMmode Should be set to AR_DEFAULT.
 */
extern void analogReference( eAnalogReference ulMode ) ;

/*
 * \brief Writes an analog value (PWM wave) to a pin.
 *
 * \param ulPin
 * \param ulValue
 */
extern void analogWrite( uint32_t ulPin, uint32_t ulValue ) ;

/*
 * \brief Reads the value from the specified analog pin.
 *
 * \param ulPin
 *
 * \return Read value from selected pin, if no error.
 */
extern uint32_t analogRead( uint32_t ulPin ) ;

/*
 * \brief Set the resolution of analogRead return values. Default is 10 bits (range from 0 to 1023).
 *
 * \param res
 */
extern void analogReadResolution(int res);

/*
 * \brief Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255).
 *
 * \param res
 */
extern void analogWriteResolution(uint8_t res);

extern void analogOutputInit( void ) ;

#ifdef __cplusplus
}
#endif
