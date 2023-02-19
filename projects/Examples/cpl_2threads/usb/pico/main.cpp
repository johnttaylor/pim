#include "../../../../application.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Stdio.h"




/// Allocate memory for the console stream pointers
Cpl::Io::Input*  g_consoleInputFdPtr;
Cpl::Io::Output* g_consoleOutputFdPtr;


/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    

    // Set the stream of the console
    g_consoleInputFdPtr  = &(Cpl::System::RP2040::getStdinStream());
    g_consoleOutputFdPtr = &(Cpl::System::RP2040::getStdoutStream());

    // Start the application
    runApplication();         // This method should never return
}

// NOT USED
void platformHook_core0_beginThread()
{
}
void platformHook_core1_beginThread()
{
}
void platformHook_core1_idleThread()
{
}
