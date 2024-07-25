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

#include "adxl345.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"

#define SECT_	"_0test"

#define BAUD_RATE   4000000     // 4MHz

#define STOP_TEST()         for ( ;; )
using namespace Driver::SPI;

#define READ_OPERATION          0x80
#define WRITE_OPERATION         0x00
#define MULTI_BYTE_TRANSFER     0x40

#define DEVICE_ID_ADDRESS       0x00
#define READ_DEVICE_ID_LEN      (1+1)
#define DEVICE_ID_INDEX         (0+1)
#define EXPECTED_DEVICE_ID      0xE5

#define BAUD_RATE_ADDRESS       0x2C
#define READWRITE_BAUD_RATE_LEN (1+1)
#define BAUD_RATE_INDEX         (0+1)
#define NEW_BAUD_RATE           (0x07)
#define RESET_BAUD_RATE         (0x0A)

#define POWER_CTL_ADDRESS       (0x2D)
#define READWRITE_POWER_CTL_LEN (1+1)
#define ENABLE_MEASUREMENTS     (0x08)

#define DATAXYZ_START_ADDRESS   0x32
#define READ_DATAXYZ_LEN        (6+1)
#define DATAX0_LO_BYTE_INDEX    (0+1)
#define DATAX1_HI_BYTE_INDEX    (1+1)
#define DATAY0_LO_BYTE_INDEX    (2+1)
#define DATAY1_HI_BYTE_INDEX    (3+1)
#define DATAZ0_LO_BYTE_INDEX    (4+1)
#define DATAZ1_HI_BYTE_INDEX    (5+1)

static inline int16_t getDataWord( uint8_t dstBuffer[READ_DATAXYZ_LEN], uint8_t loByteIndex )
{
    uint16_t data  = dstBuffer[loByteIndex];
    data          |= (dstBuffer[loByteIndex + 1]) << 8;
    return data;
}

void runtests( Driver::SPI::Master & uut, Driver::DIO::Out& cs )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("SPI Driver Test - expecting a ADXL345 Digital Accelerometer.") );

    // On start-up the chip defaults to:
    //  4-wire SPI (CPOL=1, CPHA=1)
    //  100Hz sampling rate
    //  10bit mode
    //  Samples are right-justified with sign extension
    //  Range: +/- 2g
    //  FIFO: bypassed
    //  Device ID:= 0xE5

    // Initialize ChipSelect
    bool result = cs.start( false );
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED to start the CS driver") );
        STOP_TEST();
    }

    // Start the driver
    result = uut.start( BAUD_RATE );
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED to start the SPI driver") );
        STOP_TEST();
    }

    // Read the device ID
    uint8_t srcBuffer[2] ={ 0, };
    uint8_t dstBuffer[READ_DATAXYZ_LEN]  ={ 0, };
    srcBuffer[0] = READ_OPERATION | DEVICE_ID_ADDRESS;
    cs.assertOutput();
    result       = uut.transfer( READ_DEVICE_ID_LEN, srcBuffer, dstBuffer );
    cs.deassertOutput();
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED read the device ID") );
        STOP_TEST();
    }
    if ( dstBuffer[DEVICE_ID_INDEX] != EXPECTED_DEVICE_ID )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Wrong Device ID: received=%02X, expected=%02X", dstBuffer[DEVICE_ID_INDEX], EXPECTED_DEVICE_ID) );
        STOP_TEST();
    }

    // Read the current BaudRate
    memset( srcBuffer, 0, sizeof( srcBuffer ) );
    memset( dstBuffer, 0, sizeof( dstBuffer ) );
    srcBuffer[0] = READ_OPERATION | BAUD_RATE_ADDRESS;
    cs.assertOutput();
    result         = uut.transfer( READWRITE_BAUD_RATE_LEN, srcBuffer, dstBuffer );
    cs.deassertOutput();
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED read the BaudRate register") );
        STOP_TEST();
    }
    if ( dstBuffer[BAUD_RATE_INDEX] != RESET_BAUD_RATE )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Warning: the expected 'reset' baud rate is=%02X, expected=%02X", dstBuffer[BAUD_RATE_INDEX], RESET_BAUD_RATE) );
    }

    // Set a new baud rate
    memset( srcBuffer, 0, sizeof( srcBuffer ) );
    memset( dstBuffer, 0, sizeof( dstBuffer ) );
    srcBuffer[0] = WRITE_OPERATION | BAUD_RATE_ADDRESS;
    srcBuffer[1] = NEW_BAUD_RATE;
    cs.assertOutput();
    result       = uut.transfer( READWRITE_BAUD_RATE_LEN, srcBuffer, dstBuffer );
    cs.deassertOutput();
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED WRITE the BaudRate register") );
        STOP_TEST();
    }

    // Verify the new BaudRate
    memset( srcBuffer, 0, sizeof( srcBuffer ) );
    memset( dstBuffer, 0, sizeof( dstBuffer ) );
    srcBuffer[0] = READ_OPERATION | BAUD_RATE_ADDRESS;
    cs.assertOutput();
    result       = uut.transfer( READWRITE_BAUD_RATE_LEN, srcBuffer, dstBuffer );
    cs.deassertOutput();
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED read the BaudRate register") );
        STOP_TEST();
    }
    if ( dstBuffer[BAUD_RATE_INDEX] != NEW_BAUD_RATE )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED to set a new baud rate, read=%02X, expected=%02X", dstBuffer[BAUD_RATE_INDEX], NEW_BAUD_RATE) );
    }

    // Enable measurements
    memset( srcBuffer, 0, sizeof( srcBuffer ) );
    memset( dstBuffer, 0, sizeof( dstBuffer ) );
    srcBuffer[0] = WRITE_OPERATION | POWER_CTL_ADDRESS;
    srcBuffer[1] = ENABLE_MEASUREMENTS;
    cs.assertOutput();
    result       = uut.transfer( READWRITE_POWER_CTL_LEN, srcBuffer, dstBuffer );
    cs.deassertOutput();
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED WRITE the Power_Control register") );
        STOP_TEST();
    }

    // Poll the Accelerometer
    for ( ;;)
    {
        // Read the data registers
        memset( srcBuffer, 0, sizeof( srcBuffer ) );
        memset( dstBuffer, 0, sizeof( dstBuffer ) );
        srcBuffer[0] = READ_OPERATION | MULTI_BYTE_TRANSFER | DATAXYZ_START_ADDRESS;
        cs.assertOutput();
        result       = uut.transfer( READ_DATAXYZ_LEN, srcBuffer, dstBuffer );
        cs.deassertOutput();
        if ( !result )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED read the Data registers") );
            STOP_TEST();
        }
        CPL_SYSTEM_TRACE_MSG( SECT_, ("X= %06d, Y= %06d, Z= %06d",
                                       getDataWord( dstBuffer, DATAX0_LO_BYTE_INDEX ),
                                       getDataWord( dstBuffer, DATAY0_LO_BYTE_INDEX ),
                                       getDataWord( dstBuffer, DATAZ0_LO_BYTE_INDEX )));
        Cpl::System::Api::sleep( 200 );
    }
}
