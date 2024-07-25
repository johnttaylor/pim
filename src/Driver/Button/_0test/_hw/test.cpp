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
#include "Driver/Button/PolledDebounced.h"
#include "Bsp/Api.h"

using namespace Driver::Button;

#ifndef OPTION_POLLING_INTERVAL_MS
#define OPTION_POLLING_INTERVAL_MS      10
#endif

#define SECT_   "_0test"


void runtests( Driver_Button_Hal_T& hal1,
               unsigned             numConsecutiveCounts1,
               Driver_Button_Hal_T& hal2,
               unsigned             numConsecutiveCounts2 )

{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Polled Debounced Button Driver Test.  Press some buttons...") );

    // Create the button driver instances
    PolledDebounced button1( hal1, numConsecutiveCounts1 );
    PolledDebounced button2( hal2, numConsecutiveCounts2 );
    Bsp_Api_turnOff_debug1();

    bool button1WasPressed = false;
    bool button2WasPressed = false;
    for ( ;;)
    {
        button1.sample();
        button2.sample();
        uint32_t now = Cpl::System::ElapsedTime::milliseconds();

        if ( button1.isPressed() )
        {
            if ( !button1WasPressed )
            {
                button1WasPressed = true;
                Bsp_Api_turnOn_debug1();
                CPL_SYSTEM_TRACE_MSG( SECT_, ("BUTTON1 Pressed.  timeMark = %lu", now) );
            }
        }
        else if ( button1WasPressed )
        {
            button1WasPressed = false;
            Bsp_Api_turnOff_debug1();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("BUTTON1 released. timeMark = %lu", now));
        }
    
        if ( button2.isPressed() )
        {
            if ( !button2WasPressed )
            {
                button2WasPressed = true;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("button2 Pressed.  timeMark = %lu", now) );
            }
        }
        else if ( button2WasPressed )
        {
            button2WasPressed = false;
            CPL_SYSTEM_TRACE_MSG( SECT_, ("button2 released. timeMark = %lu", now));
        }

        uint32_t later = Cpl::System::ElapsedTime::milliseconds();
        Cpl::System::Api::sleep( OPTION_POLLING_INTERVAL_MS - (later - now) );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}