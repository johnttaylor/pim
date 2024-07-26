/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/ElapsedTime.h"
#include "Driver/AIO/HalSingleInput.h"
#include "Bsp/Api.h"


#ifndef OPTION_POLLING_INTERVAL_MS
#define OPTION_POLLING_INTERVAL_MS      500
#endif

#define SECT_   "_0test"

void runtests( DriverAIOHalSingleInput_T handleInput )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("AIO Single Input Driver Test...") );

    // Loop forever
    Bsp_Api_turnOff_debug1();
    for ( ;;)
    {
        uint32_t now = Cpl::System::ElapsedTime::milliseconds();
        uint32_t bits;
        if ( !Driver_AIO_HalSingleInput_sample( handleInput, bits ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Read of ADC failed!") );
            for ( ;;);
        }
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ADC Bits = %u", bits) );

        uint32_t later = Cpl::System::ElapsedTime::milliseconds();
        Cpl::System::Api::sleep( OPTION_POLLING_INTERVAL_MS - (later - now) );
        Bsp_Api_toggle_debug1();
    }
}