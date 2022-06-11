/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Bsp/Api.h"
#include "Cpl/Io/Serial/Arduino/InputOutput.h"

#define SECT_   "bsp"


static Cpl::Io::Serial::Arduino::InputOutput fd_;
extern Cpl::Io::InputOutput& Bsp_Serial( void );



///////////////////////////////////////////
void Bsp_Api_initialize( void )
{
    // Configure the LEDs as output pins 
    pinMode( OPTION_BSP_DEBUG_LED1_INDEX, OUTPUT );
}

void Bsp_beginArduinoSerialObject( unsigned long baudrate, uint16_t frameConfig )
{
    fd_.start( baudrate, frameConfig );
    //while ( fd_.isReady() == false )
    //    ;
}

Cpl::Io::InputOutput& Bsp_Serial( void )
{
    return fd_;
}




