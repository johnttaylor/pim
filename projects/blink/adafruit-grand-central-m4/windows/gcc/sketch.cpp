/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald

  modified 2 Sep 2016
  by Arturo Guadalupi

  modified 8 Sep 2016
  by Colby Newman
*/

#include "Bsp/Api.h"
#include "task.h"

// the setup function runs once when you press reset or power the board
void setup()
{
    // Initialize the board 
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    Serial.println( "Blink Applet WITH FreeRTOS" ); Serial.println( "" );
}


// the loop function runs over and over again forever
void loop()
{
    Serial.println( "..using Ozone to debug! .. And using FreeRTOS ... blinking..." );

    digitalWrite( LED_BUILTIN, HIGH );      // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 1000 );                     // wait for a second
    digitalWrite( LED_BUILTIN, LOW );       // turn the LED off by making the voltage LOW
    vTaskDelay( 1000 );                     // wait for a second

    digitalWrite( LED_BUILTIN, HIGH );      // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 100 );                      // wait for a second
    digitalWrite( LED_BUILTIN, LOW );       // turn the LED off by making the voltage LOW
    vTaskDelay( 100 );                      // wait for a second
    digitalWrite( LED_BUILTIN, HIGH );      // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 100 );                      // wait for a second
    digitalWrite( LED_BUILTIN, LOW );       // turn the LED off by making the voltage LOW
    vTaskDelay( 100 );                      // wait for a second
}
