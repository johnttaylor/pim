#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include <stdlib.h>


extern void runtests( void );


int main( void )
{
    // Initialize the board
    Bsp_Api_initialize();
    printf( "\n**** CPL-SYSTEM TEST APPLICATION STARTED ****\n\n" );

    // Initialize CPL
    Cpl::System::Api::initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    //    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eBRIEF );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Go run the test(s) (Note: This method should never return)
    runtests();

    // I should never get here!
    for ( ;; );
    return 0;
}
