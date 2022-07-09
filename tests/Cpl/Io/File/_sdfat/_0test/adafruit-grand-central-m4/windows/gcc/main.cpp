
#include "Arduino.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/FreeRTOS/Thread.h"
#include "Cpl/Io/InputOutput.h"
#include <Adafruit_SPIFlash.h>
#include <SdFat.h>

extern void testcase_readwrite();
extern void testcase_objectapi();
extern void testcase_objectapi2();
extern void testcase_objectapi3();
extern void testcase_read1();
extern void testcase_read2();
extern void testcase_write1();
extern void testcase_write2();
extern void testcase_api1();

extern Cpl::Io::InputOutput& Bsp_Serial( void );

// file system object from SdFat
FatFileSystem fatfs;

File myFile;

Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash( &flashTransport );


// the setup function runs once when you press reset or power the board
// NOTE: FreeRTOS is RUNNING at this point
void setup()
{
    // Initialize CPL
    Cpl::System::Api::initialize();
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );


    // Make the current/main thread a CPL Thread
    Cpl::System::FreeRTOS::Thread::makeNativeMainThreadACplThread();

    // Initialize the board (for use with CPL)
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    // Artificially delay so there is time to connect to the device in order to see error message
    Cpl::System::Api::sleep( 5000 );  
    
    ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!." );
    ::Serial.println( "Running Cpl::Io::File unit tests" );
    ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!." );

    // Initialize the File System
    Bsp_beginFileSystem();

    // Run test (if there is failed -->it trip a fatal ASSERT
    ::Serial.println( "Running: readwrite() ..." ); testcase_readwrite();
    ::Serial.println( "Running: objectapi() ..." ); testcase_objectapi();
    ::Serial.println( "Running: objectapi2() ..." ); testcase_objectapi2();
    ::Serial.println( "Running: objectapi3() ..." ); testcase_objectapi3();
    ::Serial.println( "Running: read1() ..." ); testcase_read1();
    ::Serial.println( "Running: read1() ..." ); testcase_read1();
    ::Serial.println( "Running: write1() ..." ); testcase_write1();
    ::Serial.println( "Running: write2() ..." ); testcase_write2();
    ::Serial.println( "Running: api1() ..." ); testcase_api1();



    // If I get here EVERYTHING WORKED! 
    ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!." );
    ::Serial.println( "ALL Test Cases PASSED." );
    ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!." );
}



// the loop function runs over and over again forever
void loop()
{
    // Nothing to do in the loop.
    delay( 100 );
}
