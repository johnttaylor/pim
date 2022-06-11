/***************************************************************************
  This is modified/forked driver from Adafruit's original BNO055 Driver/Library

 This is a library for the BNO055 orientation sensor

  Designed specifically to work with the Adafruit BNO055 Breakout.

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by KTOWN for Adafruit Industries.

  MIT license, all text above must be included in any redistribution
 ***************************************************************************/

#include "Adafruit.h"
#include "Cpl/System/Trace.h"


#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif

#define SECT_       "driver::Imu::bno055"

using namespace Driver::Imu::Bno055;

///////////////////////////////////////////////////////////////////////////
Adafruit::Adafruit( int32_t sensorID, uint8_t address )
{
    _sensorID = sensorID;
    _address = address;
}


///////////////////////////////////////////////////////////////////////////
bool Adafruit::start( opmode_t mode )
{
    /* Enable I2C */
    Wire.begin();

    // BNO055 clock stretches for 500us or more!
#ifdef ESP8266
    Wire.setClockStretchLimit( 1000 ); // Allow for 1000us of clock stretching
#endif

    /* Make sure we have the right device */
    uint8_t id = read8( BNO055_CHIP_ID_ADDR );
    if ( id != DRIVER_IMU_BNO005_ADAFRUIT_ID )
    {
        delay( 1000 ); // hold on for boot
        id = read8( BNO055_CHIP_ID_ADDR );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("id=%d\n", id) );
        if ( id != DRIVER_IMU_BNO005_ADAFRUIT_ID ) {
            return false;  // still not? ok bail
        }
    }

    /* Switch to config mode (just in case since this is the default) */
    setMode( OPERATION_MODE_CONFIG );

    /* Reset */
    write8( BNO055_SYS_TRIGGER_ADDR, 0x20 );
    while ( read8( BNO055_CHIP_ID_ADDR ) != DRIVER_IMU_BNO005_ADAFRUIT_ID )
    {
        delay( 10 );
    }
    delay( 50 );

    /* Set to normal power mode */
    write8( BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL );
    delay( 10 );

    write8( BNO055_PAGE_ID_ADDR, 0 );

    /* Set the output units */
    /*
    uint8_t unitsel = (0 << 7) | // Orientation = Android
                      (0 << 4) | // Temperature = Celsius
                      (0 << 2) | // Euler = Degrees
                      (1 << 1) | // Gyro = Rads
                      (0 << 0);  // Accelerometer = m/s^2
    write8(BNO055_UNIT_SEL_ADDR, unitsel);
    */

    /* Configure axis mapping (see section 3.4) */
    /*
    write8(BNO055_AXIS_MAP_CONFIG_ADDR, REMAP_CONFIG_P2); // P0-P7, Default is P1
    delay(10);
    write8(BNO055_AXIS_MAP_SIGN_ADDR, REMAP_SIGN_P2); // P0-P7, Default is P1
    delay(10);
    */

    write8( BNO055_SYS_TRIGGER_ADDR, 0x0 );
    delay( 10 );
    /* Set the requested operating mode (see section 3.3) */
    setMode( mode );
    delay( 20 );

    return true;
}

void Adafruit::setMode( opmode_t mode )
{
    _mode = mode;
    write8( BNO055_OPR_MODE_ADDR, _mode );
    delay( 30 );
}

void Adafruit::setExtCrystalUse( bool usextal )
{
    opmode_t modeback = _mode;

    /* Switch to config mode (just in case since this is the default) */
    setMode( OPERATION_MODE_CONFIG );
    delay( 25 );
    write8( BNO055_PAGE_ID_ADDR, 0 );
    if ( usextal ) {
        write8( BNO055_SYS_TRIGGER_ADDR, 0x80 );
    }
    else {
        write8( BNO055_SYS_TRIGGER_ADDR, 0x00 );
    }
    delay( 10 );
    /* Set the requested operating mode (see section 3.3) */
    setMode( modeback );
    delay( 20 );
}

void Adafruit::getSystemStatus( uint8_t *system_status, uint8_t *self_test_result, uint8_t *system_error )
{
    write8( BNO055_PAGE_ID_ADDR, 0 );

    if ( system_status != 0 )
        *system_status    = read8( BNO055_SYS_STAT_ADDR );

    if ( self_test_result != 0 )
        *self_test_result = read8( BNO055_SELFTEST_RESULT_ADDR );


    if ( system_error != 0 )
        *system_error     = read8( BNO055_SYS_ERR_ADDR );

    delay( 200 );
}

void Adafruit::getRevInfo( rev_info_t* info )
{
    uint8_t a, b;

    memset( info, 0, sizeof( rev_info_t ) );

    /* Check the accelerometer revision */
    info->accel_rev = read8( BNO055_ACCEL_REV_ID_ADDR );

    /* Check the magnetometer revision */
    info->mag_rev   = read8( BNO055_MAG_REV_ID_ADDR );

    /* Check the gyroscope revision */
    info->gyro_rev  = read8( BNO055_GYRO_REV_ID_ADDR );

    /* Check the SW revision */
    info->bl_rev    = read8( BNO055_BL_REV_ID_ADDR );

    a = read8( BNO055_SW_REV_ID_LSB_ADDR );
    b = read8( BNO055_SW_REV_ID_MSB_ADDR );
    info->sw_rev = (((uint16_t) b) << 8) | ((uint16_t) a);
}

void Adafruit::getCalibration( uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag ) {
    uint8_t calData = read8( BNO055_CALIB_STAT_ADDR );
    if ( sys != NULL ) {
        *sys = (calData >> 6) & 0x03;
    }
    if ( gyro != NULL ) {
        *gyro = (calData >> 4) & 0x03;
    }
    if ( accel != NULL ) {
        *accel = (calData >> 2) & 0x03;
    }
    if ( mag != NULL ) {
        *mag = calData & 0x03;
    }
}

int8_t Adafruit::getTemp( void )
{
    int8_t temp = (int8_t) (read8( BNO055_TEMP_ADDR ));
    return temp;
}

Adafruit::raw_vector_t Adafruit::getRawVector( vector_type_t vector_type )
{
    raw_vector_t xyz;
    uint8_t buffer[6];
    memset( buffer, 0, 6 );

    /* Read vector data (6 bytes) */
    readLen( (reg_t) vector_type, buffer, 6 );

    xyz.x = ((int16_t) buffer[0]) | (((int16_t) buffer[1]) << 8);
    xyz.y = ((int16_t) buffer[2]) | (((int16_t) buffer[3]) << 8);
    xyz.z = ((int16_t) buffer[4]) | (((int16_t) buffer[5]) << 8);

    return xyz;
}

Adafruit::vector_t Adafruit::convert( vector_type_t vector_type, raw_vector_t raw_xyz )
{
    vector_t     xyz;

    /* Convert the value to an appropriate range (section 3.6.4) */
    /* and assign the value to the Vector type */
    switch ( vector_type )
    {
    case VECTOR_MAGNETOMETER:
        /* 1uT = 16 LSB */
        xyz.x = ((float) raw_xyz.x) / 16.0;
        xyz.y = ((float) raw_xyz.y) / 16.0;
        xyz.z = ((float) raw_xyz.z) / 16.0;
        break;

    case VECTOR_GYROSCOPE:
        /* 1dps = 16 LSB */
        xyz.x = ((float) raw_xyz.x) / 16.0;
        xyz.y = ((float) raw_xyz.y) / 16.0;
        xyz.z = ((float) raw_xyz.z) / 16.0;
        break;

    case VECTOR_EULER:
        /* 1 degree = 16 LSB */
        xyz.x = ((float) raw_xyz.x) / 16.0;
        xyz.y = ((float) raw_xyz.y) / 16.0;
        xyz.z = ((float) raw_xyz.z) / 16.0;
        break;

    case VECTOR_ACCELEROMETER:
    case VECTOR_LINEARACCEL:
    case VECTOR_GRAVITY:
        /* 1m/s^2 = 100 LSB */
        xyz.x = ((float) raw_xyz.x) / 100.0;
        xyz.y = ((float) raw_xyz.y) / 100.0;
        xyz.z = ((float) raw_xyz.z) / 100.0;
        break;
    }

    return xyz;
}

Adafruit::raw_quat_t Adafruit::getRawQuat( void )
{
    raw_quat_t quat;
    uint8_t buffer[8];
    memset( buffer, 0, 8 );

    /* Read quat data (8 bytes) */
    readLen( BNO055_QUATERNION_DATA_W_LSB_ADDR, buffer, 8 );
    quat.w = (((uint16_t) buffer[1]) << 8) | ((uint16_t) buffer[0]);
    quat.x = (((uint16_t) buffer[3]) << 8) | ((uint16_t) buffer[2]);
    quat.y = (((uint16_t) buffer[5]) << 8) | ((uint16_t) buffer[4]);
    quat.z = (((uint16_t) buffer[7]) << 8) | ((uint16_t) buffer[6]);

    return quat;
}

Adafruit::quat_t Adafruit::convert( Adafruit::raw_quat_t raw_quat )
{
    quat_t quat;

    /* Assign to Quaternion */
    /* See http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_12~1.pdf
       3.6.5.5 Orientation (Quaternion)  */
    const float scale = (1.0 / (1 << 14));
    quat.w = scale * raw_quat.w;
    quat.x = scale * raw_quat.x;
    quat.y = scale * raw_quat.y;
    quat.z = scale * raw_quat.z;

    return quat;
}

bool Adafruit::getSensorOffsets( uint8_t* calibData )
{
    if ( isFullyCalibrated() )
    {
        opmode_t lastMode = _mode;
        setMode( OPERATION_MODE_CONFIG );

        readLen( ACCEL_OFFSET_X_LSB_ADDR, calibData, DRIVER_BOSCH_BNO005_ADAFRUIT_NUM_CAL_OFFSET_REGISTERS );

        setMode( lastMode );
        return true;
    }
    return false;
}

bool Adafruit::getSensorOffsets( calibration_offsets_t &offsets_type )
{
    if ( isFullyCalibrated() )
    {
        opmode_t lastMode = _mode;
        setMode( OPERATION_MODE_CONFIG );
        delay( 25 );

        offsets_type.accel_offset_x = (read8( ACCEL_OFFSET_X_MSB_ADDR ) << 8) | (read8( ACCEL_OFFSET_X_LSB_ADDR ));
        offsets_type.accel_offset_y = (read8( ACCEL_OFFSET_Y_MSB_ADDR ) << 8) | (read8( ACCEL_OFFSET_Y_LSB_ADDR ));
        offsets_type.accel_offset_z = (read8( ACCEL_OFFSET_Z_MSB_ADDR ) << 8) | (read8( ACCEL_OFFSET_Z_LSB_ADDR ));

        offsets_type.gyro_offset_x = (read8( GYRO_OFFSET_X_MSB_ADDR ) << 8) | (read8( GYRO_OFFSET_X_LSB_ADDR ));
        offsets_type.gyro_offset_y = (read8( GYRO_OFFSET_Y_MSB_ADDR ) << 8) | (read8( GYRO_OFFSET_Y_LSB_ADDR ));
        offsets_type.gyro_offset_z = (read8( GYRO_OFFSET_Z_MSB_ADDR ) << 8) | (read8( GYRO_OFFSET_Z_LSB_ADDR ));

        offsets_type.mag_offset_x = (read8( MAG_OFFSET_X_MSB_ADDR ) << 8) | (read8( MAG_OFFSET_X_LSB_ADDR ));
        offsets_type.mag_offset_y = (read8( MAG_OFFSET_Y_MSB_ADDR ) << 8) | (read8( MAG_OFFSET_Y_LSB_ADDR ));
        offsets_type.mag_offset_z = (read8( MAG_OFFSET_Z_MSB_ADDR ) << 8) | (read8( MAG_OFFSET_Z_LSB_ADDR ));

        offsets_type.accel_radius = (read8( ACCEL_RADIUS_MSB_ADDR ) << 8) | (read8( ACCEL_RADIUS_LSB_ADDR ));
        offsets_type.mag_radius = (read8( MAG_RADIUS_MSB_ADDR ) << 8) | (read8( MAG_RADIUS_LSB_ADDR ));

        setMode( lastMode );
        return true;
    }
    return false;
}


void Adafruit::setSensorOffsets( const uint8_t* calibData )
{
    opmode_t lastMode = _mode;
    setMode( OPERATION_MODE_CONFIG );
    delay( 25 );

    /* A writeLen() would make this much cleaner */
    write8( ACCEL_OFFSET_X_LSB_ADDR, calibData[0] );
    write8( ACCEL_OFFSET_X_MSB_ADDR, calibData[1] );
    write8( ACCEL_OFFSET_Y_LSB_ADDR, calibData[2] );
    write8( ACCEL_OFFSET_Y_MSB_ADDR, calibData[3] );
    write8( ACCEL_OFFSET_Z_LSB_ADDR, calibData[4] );
    write8( ACCEL_OFFSET_Z_MSB_ADDR, calibData[5] );

    write8( GYRO_OFFSET_X_LSB_ADDR, calibData[6] );
    write8( GYRO_OFFSET_X_MSB_ADDR, calibData[7] );
    write8( GYRO_OFFSET_Y_LSB_ADDR, calibData[8] );
    write8( GYRO_OFFSET_Y_MSB_ADDR, calibData[9] );
    write8( GYRO_OFFSET_Z_LSB_ADDR, calibData[10] );
    write8( GYRO_OFFSET_Z_MSB_ADDR, calibData[11] );

    write8( MAG_OFFSET_X_LSB_ADDR, calibData[12] );
    write8( MAG_OFFSET_X_MSB_ADDR, calibData[13] );
    write8( MAG_OFFSET_Y_LSB_ADDR, calibData[14] );
    write8( MAG_OFFSET_Y_MSB_ADDR, calibData[15] );
    write8( MAG_OFFSET_Z_LSB_ADDR, calibData[16] );
    write8( MAG_OFFSET_Z_MSB_ADDR, calibData[17] );

    write8( ACCEL_RADIUS_LSB_ADDR, calibData[18] );
    write8( ACCEL_RADIUS_MSB_ADDR, calibData[19] );

    write8( MAG_RADIUS_LSB_ADDR, calibData[20] );
    write8( MAG_RADIUS_MSB_ADDR, calibData[21] );

    setMode( lastMode );
}

void Adafruit::setSensorOffsets( const calibration_offsets_t &offsets_type )
{
    opmode_t lastMode = _mode;
    setMode( OPERATION_MODE_CONFIG );
    delay( 25 );

    write8( ACCEL_OFFSET_X_LSB_ADDR, (offsets_type.accel_offset_x) & 0x0FF );
    write8( ACCEL_OFFSET_X_MSB_ADDR, (offsets_type.accel_offset_x >> 8) & 0x0FF );
    write8( ACCEL_OFFSET_Y_LSB_ADDR, (offsets_type.accel_offset_y) & 0x0FF );
    write8( ACCEL_OFFSET_Y_MSB_ADDR, (offsets_type.accel_offset_y >> 8) & 0x0FF );
    write8( ACCEL_OFFSET_Z_LSB_ADDR, (offsets_type.accel_offset_z) & 0x0FF );
    write8( ACCEL_OFFSET_Z_MSB_ADDR, (offsets_type.accel_offset_z >> 8) & 0x0FF );

    write8( GYRO_OFFSET_X_LSB_ADDR, (offsets_type.gyro_offset_x) & 0x0FF );
    write8( GYRO_OFFSET_X_MSB_ADDR, (offsets_type.gyro_offset_x >> 8) & 0x0FF );
    write8( GYRO_OFFSET_Y_LSB_ADDR, (offsets_type.gyro_offset_y) & 0x0FF );
    write8( GYRO_OFFSET_Y_MSB_ADDR, (offsets_type.gyro_offset_y >> 8) & 0x0FF );
    write8( GYRO_OFFSET_Z_LSB_ADDR, (offsets_type.gyro_offset_z) & 0x0FF );
    write8( GYRO_OFFSET_Z_MSB_ADDR, (offsets_type.gyro_offset_z >> 8) & 0x0FF );

    write8( MAG_OFFSET_X_LSB_ADDR, (offsets_type.mag_offset_x) & 0x0FF );
    write8( MAG_OFFSET_X_MSB_ADDR, (offsets_type.mag_offset_x >> 8) & 0x0FF );
    write8( MAG_OFFSET_Y_LSB_ADDR, (offsets_type.mag_offset_y) & 0x0FF );
    write8( MAG_OFFSET_Y_MSB_ADDR, (offsets_type.mag_offset_y >> 8) & 0x0FF );
    write8( MAG_OFFSET_Z_LSB_ADDR, (offsets_type.mag_offset_z) & 0x0FF );
    write8( MAG_OFFSET_Z_MSB_ADDR, (offsets_type.mag_offset_z >> 8) & 0x0FF );

    write8( ACCEL_RADIUS_LSB_ADDR, (offsets_type.accel_radius) & 0x0FF );
    write8( ACCEL_RADIUS_MSB_ADDR, (offsets_type.accel_radius >> 8) & 0x0FF );

    write8( MAG_RADIUS_LSB_ADDR, (offsets_type.mag_radius) & 0x0FF );
    write8( MAG_RADIUS_MSB_ADDR, (offsets_type.mag_radius >> 8) & 0x0FF );

    setMode( lastMode );
}

bool Adafruit::isFullyCalibrated( void )
{
    uint8_t system, gyro, accel, mag;
    getCalibration( &system, &gyro, &accel, &mag );
    if ( system < 3 || gyro < 3 || accel < 3 || mag < 3 )
        return false;
    return true;
}


///////////////////////////////////////////////////////////////////////////
bool Adafruit::write8( reg_t reg, byte value )
{
    Wire.beginTransmission( _address );
#if ARDUINO >= 100
    Wire.write( (uint8_t) reg );
    Wire.write( (uint8_t) value );
#else
    Wire.send( reg );
    Wire.send( value );
#endif
    Wire.endTransmission();

    /* ToDo: Check for error! */
    return true;
}

uint8_t Adafruit::read8( reg_t reg )
{
    byte value = 0;

    Wire.beginTransmission( _address );
#if ARDUINO >= 100
    Wire.write( (uint8_t) reg );
#else
    Wire.send( reg );
#endif
    Wire.endTransmission();
    Wire.requestFrom( _address, (uint8_t) 1 );
#if ARDUINO >= 100
    value = Wire.read();
#else
    value = Wire.receive();
#endif

    return value;
}

bool Adafruit::readLen( reg_t reg, uint8_t * buffer, uint8_t len )
{
    Wire.beginTransmission( _address );
#if ARDUINO >= 100
    Wire.write( (uint8_t) reg );
#else
    Wire.send( reg );
#endif
    Wire.endTransmission();
    Wire.requestFrom( _address, len );

    for ( uint8_t i = 0; i < len; i++ )
    {
#if ARDUINO >= 100
        buffer[i] = Wire.read();
#else
        buffer[i] = Wire.receive();
#endif
    }

    /* ToDo: Check for errors! */
    return true;
}
