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

#include "colony_config.h"
#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"
#include "Bsp/Api.h"
#include "Cpl/Text/FString.h"
#include <string.h>



// MUST be supplied by main.cpp. Return the current elapsed time in Milliseconds
extern uint32_t elapsedTimeMs();

// Export so main() can call me.
extern void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


////////////////////////////////////////////////////////////////////////////////

#ifndef MAX_INPUT
#define MAX_INPUT		1024
#endif

static char  inbuffer[MAX_INPUT + 1];
static Cpl::Text::FString<128> tmpString;

void test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    //printf( "\n\n**** THIS SHOULD NOT DISPLAY ***\n\n" );
    outfd.write( "\n== Testing: Cpl::Io stream via a serial device..." );
    outfd.write( "\n== Board will echo back input character once a newline\nhas been received." );
    uint32_t startTime = elapsedTimeMs();
    Bsp_Api_turnOn_debug1();

    while ( true )
    {
        // Toggle my LED
        uint32_t now = elapsedTimeMs();
        if ( now - startTime > 1000 )
        {
            startTime = now;
            Bsp_Api_toggle_debug1();
        }

        int bytesRead  = 0;
        if ( !infd.read( inbuffer, MAX_INPUT, bytesRead ) )
        {
            tmpString.format( "\n**** ERROR occurred while reading input stream (requested bytes=%d)\n", MAX_INPUT );
            outfd.write( tmpString );
        }
        else
        {
            if ( bytesRead > 0 )
            {
                // Ensure the input data is null terminated
                inbuffer[bytesRead] = '\0';
                tmpString.format( "\nECHO: len=%d [", bytesRead );
                outfd.write( tmpString );
                outfd.write( inbuffer );
                outfd.write( "]\n" );
            }
        }
    }
}
