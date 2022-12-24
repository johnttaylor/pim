#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Driver/NV/_0test/test.h"
#include "Driver/NV/File/Cpl/Api.h"
#include "Driver/NV/Gang.h"

static Driver::NV::File::Cpl::Api driver1_( 2, 64, "eeprom1.bin" );
static Driver::NV::File::Cpl::Api driver2_( 4, 64, "eeprom2.bin" );
static Driver::NV::File::Cpl::Api driver3_( 2, 128, "eeprom3.bin" );
static Driver::NV::Api* drivers_[] ={ &driver1_, &driver2_, &driver3_, 0 };
static Driver::NV::Gang uut_( drivers_ );

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();
    Cpl::System::Api::enableScheduling();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

    // Run the test(s)
    return runtests( uut_, 10, 64, 640 );
}
