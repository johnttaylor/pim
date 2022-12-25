#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Driver/NV/_0test/test.h"
#include "Driver/NV/File/Cpl/Api.h"

static Driver::NV::File::Cpl::Api uut_;

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();
    Cpl::System::Api::enableScheduling();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

    // Run the test(s)
    return runtests( uut_,
                     OPTION_DRIVER_NV_FILE_CPL_NUM_PAGES,
                     OPTION_DRIVER_NV_FILE_CPL_BYTES_PER_PAGE,
                     OPTION_DRIVER_NV_FILE_CPL_NUM_PAGES * OPTION_DRIVER_NV_FILE_CPL_BYTES_PER_PAGE );
}
