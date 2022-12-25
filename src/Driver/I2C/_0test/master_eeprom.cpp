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
/** @file */

#include "master_eeprom.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"

#define SECT_	"_0test"

#define STOP_TEST()         for ( ;; )
using namespace Driver::I2C;

void runtests( Driver::I2C::Master& uut, uint8_t device7BitAddress )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("I2C Driver Test - expecting a CAT24C512 (or compatible) EEPROM peripheral device.") );

    // Verify not-started is bad thing
    uint8_t buffer[10];
    auto result = uut.readFromDevice( device7BitAddress, 1, buffer );
    if ( result == Driver::I2C::Master::eNOT_STARTED )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] driver not started", device7BitAddress) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] unexpected result code: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Start the driver
    bool started = uut.start();
    if ( !started )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] uut.start() failed", device7BitAddress ) );
        STOP_TEST();
    }

    // Read a byte from the EEPROM to see if it connected
    result = uut.readFromDevice( device7BitAddress, 1, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Slave device is present", device7BitAddress) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Slave device did not respond: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Write some bytes to eeprom
    buffer[0] = 0;  // Start writing at offset zero
    buffer[1] = 0;
    buffer[2] = 2;
    buffer[3] = 3;
    buffer[4] = 4;
    result = uut.writeToDevice( device7BitAddress, 2 + 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Wrote 3 bytes to offset 0", device7BitAddress) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to write 3 bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Wait for write to complete (add it resets the read-pointer)
    bool writeCompleted = false;
    for ( int i=0; i < 10; i++ )
    {
        buffer[0] = 0;  // Start writing at offset zero
        buffer[1] = 0;
        result = uut.writeToDevice( device7BitAddress, 2, buffer );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            writeCompleted = true;
            break;
        }
        Cpl::System::Api::sleep( 100 );
    }
    if ( !writeCompleted )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] EEPROM not responding after write", device7BitAddress, result) );
        STOP_TEST();
    }

    // Read back the bytes
    result = uut.readFromDevice( device7BitAddress, 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Read 3 bytes to offset 0. %02x %02x %02x | 02 03 04", device7BitAddress, buffer[0], buffer[1], buffer[2]) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to read 3 bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Write some more bytes to eeprom
    buffer[0] = 0;  // Start writing at offset zero
    buffer[1] = 0;
    buffer[2] = 22;
    buffer[3] = 33;
    buffer[4] = 44;
    result = uut.writeToDevice( device7BitAddress, 2 + 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Wrote 3 more bytes to offset 0", device7BitAddress) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to write 3 more bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Wait for write to complete (add it resets the read-pointer)
    writeCompleted = false;
    for ( int i=0; i < 10; i++ )
    {
        buffer[0] = 0;  // Start writing at offset zero
        buffer[1] = 0;
        result = uut.writeToDevice( device7BitAddress, 2, buffer );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            writeCompleted = true;
            break;
        }
        Cpl::System::Api::sleep( 100 );
    }
    if ( !writeCompleted )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] EEPROM not responding after write", device7BitAddress, result) );
        STOP_TEST();
    }
    
    // Read back the bytes
    result = uut.readFromDevice( device7BitAddress, 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Read 3 more bytes to offset 0. %02x %02x %02x | 16 21 2c", device7BitAddress, buffer[0], buffer[1], buffer[2]) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to read 3 more bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }


    // Do not return once the test has completed
    CPL_SYSTEM_TRACE_MSG( SECT_, ("ALL TESTS COMPLETED SUCCESFULLY") );
    STOP_TEST();
}
