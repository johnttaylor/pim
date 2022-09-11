#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/FreeRTOS/Thread.h"
#include "Cpl/Io/InputOutput.h"
#include "Arduino.h"
#include "debug.h"

extern void runtests( void );
extern void testsetup(void);

extern Cpl::Io::InputOutput& Bsp_Serial( void );

#define SECT_       "_0test"

void setup( void )
{
    // Initialize the board (for use with CPL)
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );
    
    // Initialize CPL
    Cpl::System::Api::initialize();
    

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    //CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eNONE );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    //CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );
}


void loop( void )
{
    // Make the current/main thread a CPL Thread
    Cpl::System::FreeRTOS::Thread::makeNativeMainThreadACplThread();

    // Go run the test(s) 
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Trace: Main Arduino loop") );
    dbgMemInfo();
    runtests();
    delay( 1000 );
}


