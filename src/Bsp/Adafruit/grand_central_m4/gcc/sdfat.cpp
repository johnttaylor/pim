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
#include "Cpl/Io/File/Arduino/_sdfat/Private_.h"
#include <Arduino.h>
#include <Adafruit_SPIFlash.h>
#include <SdFat.h>


#define SECT_   "bsp"


static Adafruit_FlashTransport_QSPI flashTransport;
static Adafruit_SPIFlash flash( &flashTransport );

FatFileSystem g_arduino_sdfat_fs;


///////////////////////////////////////////
void Bsp_beginFileSystem( void )
{
    // Initialize flash library and check its chip ID.
    if ( !flash.begin() ) {
        ::Serial.println( "Error, failed to initialize flash chip!" );
        while ( 1 );
    }

    // First call begin to mount the filesystem.  Check that it returns true
    // to make sure the filesystem was mounted.
    if ( !g_arduino_sdfat_fs.begin( &flash ) ) {
        ::Serial.println( "Error, failed to mount newly formatted filesystem!" );
        ::Serial.println( "Was the flash chip formatted with the SdFat_format example?" );
        while ( 1 ) yield();
    }
}

