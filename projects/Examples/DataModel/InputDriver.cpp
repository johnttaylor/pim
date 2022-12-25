/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "colony_config.h"
#include "InputDriver.h"
#include <stdlib.h>
#include <time.h>

#ifndef OPTION_INPUT_DRIVER_SAMPLE_TIME_MS
#define OPTION_INPUT_DRIVER_SAMPLE_TIME_MS      10
#endif

#ifndef OPTION_INPUT_DRIVER_UPPER_VALUE
#define OPTION_INPUT_DRIVER_UPPER_VALUE         1000
#endif

#ifndef OPTION_INPUT_DRIVER_LOWER_VALUE
#define OPTION_INPUT_DRIVER_LOWER_VALUE         1
#endif

///////////////////////////////
InputDriver::InputDriver( Cpl::Dm::MailboxServer& myMbox, 
                          Cpl::Dm::Mp::Uint32&    mpToUpdate )
    : Cpl::Itc::CloseSync( myMbox )
    , Cpl::System::Timer( myMbox )
    , m_mpSample( mpToUpdate )
    , m_opened( false )
{
}

///////////////////////////////

// Note: This method runs in the 'Driver' thread
void InputDriver::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( !m_opened )
    {
        // Housekeeping
        m_opened = true;
        srand( (unsigned) time( NULL ) );  // Seed the random number generator based on the current time

        // Generate the first sample
        expired();  
    }

    // The request method is an ITC call -->the server is required to 'return the message' to the client
    msg.returnToSender();
}

// Note: This method runs in the 'Driver' thread
void InputDriver::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;
        Timer::stop();
    }

    // The request method is an ITC call -->the server is required to 'return the message' to the client
    msg.returnToSender();
}



///////////////////////////////

// Note: This method runs in the 'Driver' thread
void InputDriver::expired( void ) noexcept
{
    // Generate a random between 1 and 1000
    uint32_t sample = (rand() % (OPTION_INPUT_DRIVER_UPPER_VALUE - OPTION_INPUT_DRIVER_LOWER_VALUE + 1)) + OPTION_INPUT_DRIVER_LOWER_VALUE;

    // Update the MP
    m_mpSample.write( sample );

    // Restart my sampling timer
    Timer::start( OPTION_INPUT_DRIVER_SAMPLE_TIME_MS );
}




