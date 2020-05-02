/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Bsp/Api.h"
#include "Cpl/Io/Serial/Adafruit/Nrf5/InputOutput.h"
#include "Cpl/System/Trace.h"
#include "Uart.h"
#include "wiring_analog.h"
#include "delay.h"
#include <ios>

#define SECT_   "bsp"

#ifndef OPTION_BSP_BATTERY_ADC_PIN
#define OPTION_BSP_BATTERY_ADC_PIN              VBAT_PIN
#endif

#define VBAT_PIN          (A7)
#define VBAT_MV_PER_LSB   (0.73242188F)     // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_DIVIDER      (0.71275837F)     // 2M + 0.806M voltage divider on VBAT = (2M / (0.806M + 2M))
#define VBAT_DIVIDER_COMP (1/VBAT_DIVIDER)  // Compensation factor for the VBAT divider



static Cpl::Io::Serial::Adafruit::Nrf5::InputOutput fd_( Serial );    // Note: 'Serial' is the Arduino primary uart object!
extern Cpl::Io::InputOutput& Bsp_Serial( void );



///////////////////////////////////////////
void Bsp_Api_initialize( void )
{
    // Configure the LEDs as output pins 
    pinMode( PIN_LED1, OUTPUT );
    pinMode( PIN_LED2, OUTPUT );
}

void Bsp_beginArduinoSerialObject( unsigned long baudrate, uint16_t frameConfig )
{
    fd_.start( baudrate, frameConfig );
    while ( fd_.isReady() == false )
        ;
}

Cpl::Io::InputOutput& Bsp_Serial( void )
{
    return fd_;
}

void Bsp_readBatteryAdc( float* batteryMvP, uint8_t* batteryPercentageP, float* adcMvP, int* rawAdcP )
{
    // Set the analog reference to 3.0V (default = 3.6V)
    analogReference( AR_INTERNAL_3_0 );

    // Set the resolution to 12-bit (0..4095)
    analogReadResolution( 12 ); // Can be 8, 10, 12 or 14

    // Let the ADC settle
    delay( 1 );

    // Get the raw 12-bit, 0..3000mV ADC value
    int   vbat_raw = analogRead( OPTION_BSP_BATTERY_ADC_PIN );
    float vbat_mv  = vbat_raw * VBAT_MV_PER_LSB;
    
    CPL_SYSTEM_TRACE_MSG( SECT_, ("vbat_raw=%d, vbat_mv=%d", vbat_raw, (int) (vbat_mv)) );

    // Convert from raw mv to percentage (based on LIPO chemistry)
    if ( batteryPercentageP )
    {
        *batteryPercentageP = Bsp_batteryAdcMvToPercent( vbat_mv );
    }

    // Convert the raw value to compensated mv, taking the resistor-
    // divider into account (providing the actual LIPO voltage)
    // ADC range is 0..3000mV and resolution is 12-bit (0..4095),
    // VBAT voltage divider is 2M + 0.806M, which needs to be added back
    if ( batteryMvP )
    {
        *batteryMvP = (float) vbat_mv * VBAT_DIVIDER_COMP;
    }

    // Return the ADC result in millivolts (value is NOT compensated for the voltage divider)
    if ( adcMvP )
    {
        *adcMvP = vbat_mv;
    }

    // Return raw ADC counts
    if ( rawAdcP )
    {
        *rawAdcP = vbat_raw;
    }
}

uint8_t Bsp_batteryAdcMvToPercent( float mvolts )
{
    uint8_t battery_level;

    if ( mvolts >= 3000 )
    {
        battery_level = 100;
    }
    else if ( mvolts > 2900 )
    {
        battery_level = 100 - ((3000 - mvolts) * 58) / 100;
    }
    else if ( mvolts > 2740 )
    {
        battery_level = 42 - ((2900 - mvolts) * 24) / 160;
    }
    else if ( mvolts > 2440 )
    {
        battery_level = 18 - ((2740 - mvolts) * 12) / 300;
    }
    else if ( mvolts > 2100 )
    {
        battery_level = 6 - ((2440 - mvolts) * 6) / 340;
    }
    else
    {
        battery_level = 0;
    }

    return battery_level;
}


////////////////////////////////////////////////////
/*
** Method stubs to satisfy the linker -->NOTE: C++ Streams are NOT supported by this BSP
*/
//void std::ios_base::Init::_S_ios_destroy()
//{
//}
//
//void std::ios_base::Init::_S_ios_create( bool  )
//{
//}

std::ios_base::Init::Init()
{
}

std::ios_base::Init::~Init()
{
}

void *operator new(size_t size, std::nothrow_t const&){
  return rtos_malloc( size );
}

void *operator new[]( size_t size, std::nothrow_t const& ) {
    return rtos_malloc( size );
}

const std::nothrow_t std::nothrow;
