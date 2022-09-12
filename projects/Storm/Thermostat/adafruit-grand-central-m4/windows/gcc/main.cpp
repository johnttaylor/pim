
#include "Arduino.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/FreeRTOS/Thread.h"
#include "Storm/Thermostat/Main/Main.h"
#include "Cpl/Io/InputOutput.h"
#include <Adafruit_NeoPixel.h>

extern Cpl::Io::InputOutput& Bsp_Serial( void );

// NeoPixel Configuration: (Values supplied by the build script!)
#define PIN            BUILD_OPT_PIN        
#define NUMPIXELS      BUILD_OPT_NUM_PIXELS 
#define NEO_TYPE       BUILD_OPT_NEO_TYPE   

// Expose NeoPixel driver
extern Adafruit_NeoPixel g_pixels;

Adafruit_NeoPixel g_pixels = Adafruit_NeoPixel( NUMPIXELS, PIN, NEO_TYPE + NEO_KHZ800 );


// the setup function runs once when you press reset or power the board
// NOTE: FreeRTOS is RUNNING at this point
void setup()
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Make the current/main thread a CPL Thread
    Cpl::System::FreeRTOS::Thread::makeNativeMainThreadACplThread();

    // Initialize the board (for use with CPL)
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    // Initialize the File System
    Bsp_beginFileSystem();

    // Initialize the NeoPixel shield (and CLEAR all LEDs)
    g_pixels.begin();
    for ( int i=0; i < NUMPIXELS; i++ )
    {
        g_pixels.setPixelColor( i, g_pixels.Color( 0, 0, 0, 0 ) );
    }
    g_pixels.show();
    delay( 250 ); // Delay for a period of time (in milliseconds).
}


// the loop function runs over and over again forever
void loop()
{
    // Run the application (Note: This method does not return)
    runTheApplication( Bsp_Serial(), Bsp_Serial() );
}
