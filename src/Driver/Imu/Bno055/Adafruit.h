#ifndef Driver_Imu_Bno044_Adafruit_h_
#define Driver_Imu_Bno044_Adafruit_h_
/***************************************************************************
  This is modified/forked driver from Adafruit's original BNO055 Driver/Library

  This is a library for the Bosch BNO055 orientation sensor

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

#include <stdint.h>
#include "Driver/Imu/Vector.h"

#ifndef DOXYGEN_WILL_SKIP_THIS

#define DRIVER_IMU_BNO005_ADAFRUIT_ADDRESS_A                  (0x28)
#define DRIVER_IMU_BNO005_ADAFRUIT_ADDRESS_B                  (0x29)
#define DRIVER_IMU_BNO005_ADAFRUIT_ID                         (0xA0)

#define DRIVER_BOSCH_BNO005_ADAFRUIT_NUM_CAL_OFFSET_REGISTERS   (22)

namespace Driver {
namespace Imu {
namespace Bno055 {

class Adafruit
{
public:
    typedef enum
    {
        /* Page id register definition */
        BNO055_PAGE_ID_ADDR                                     = 0X07,

        /* PAGE0 REGISTER DEFINITION START*/
        BNO055_CHIP_ID_ADDR                                     = 0x00,
        BNO055_ACCEL_REV_ID_ADDR                                = 0x01,
        BNO055_MAG_REV_ID_ADDR                                  = 0x02,
        BNO055_GYRO_REV_ID_ADDR                                 = 0x03,
        BNO055_SW_REV_ID_LSB_ADDR                               = 0x04,
        BNO055_SW_REV_ID_MSB_ADDR                               = 0x05,
        BNO055_BL_REV_ID_ADDR                                   = 0X06,

        /* Accel data register */
        BNO055_ACCEL_DATA_X_LSB_ADDR                            = 0X08,
        BNO055_ACCEL_DATA_X_MSB_ADDR                            = 0X09,
        BNO055_ACCEL_DATA_Y_LSB_ADDR                            = 0X0A,
        BNO055_ACCEL_DATA_Y_MSB_ADDR                            = 0X0B,
        BNO055_ACCEL_DATA_Z_LSB_ADDR                            = 0X0C,
        BNO055_ACCEL_DATA_Z_MSB_ADDR                            = 0X0D,

        /* Mag data register */
        BNO055_MAG_DATA_X_LSB_ADDR                              = 0X0E,
        BNO055_MAG_DATA_X_MSB_ADDR                              = 0X0F,
        BNO055_MAG_DATA_Y_LSB_ADDR                              = 0X10,
        BNO055_MAG_DATA_Y_MSB_ADDR                              = 0X11,
        BNO055_MAG_DATA_Z_LSB_ADDR                              = 0X12,
        BNO055_MAG_DATA_Z_MSB_ADDR                              = 0X13,

        /* Gyro data registers */
        BNO055_GYRO_DATA_X_LSB_ADDR                             = 0X14,
        BNO055_GYRO_DATA_X_MSB_ADDR                             = 0X15,
        BNO055_GYRO_DATA_Y_LSB_ADDR                             = 0X16,
        BNO055_GYRO_DATA_Y_MSB_ADDR                             = 0X17,
        BNO055_GYRO_DATA_Z_LSB_ADDR                             = 0X18,
        BNO055_GYRO_DATA_Z_MSB_ADDR                             = 0X19,

        /* Euler data registers */
        BNO055_EULER_H_LSB_ADDR                                 = 0X1A,
        BNO055_EULER_H_MSB_ADDR                                 = 0X1B,
        BNO055_EULER_R_LSB_ADDR                                 = 0X1C,
        BNO055_EULER_R_MSB_ADDR                                 = 0X1D,
        BNO055_EULER_P_LSB_ADDR                                 = 0X1E,
        BNO055_EULER_P_MSB_ADDR                                 = 0X1F,

        /* Quaternion data registers */
        BNO055_QUATERNION_DATA_W_LSB_ADDR                       = 0X20,
        BNO055_QUATERNION_DATA_W_MSB_ADDR                       = 0X21,
        BNO055_QUATERNION_DATA_X_LSB_ADDR                       = 0X22,
        BNO055_QUATERNION_DATA_X_MSB_ADDR                       = 0X23,
        BNO055_QUATERNION_DATA_Y_LSB_ADDR                       = 0X24,
        BNO055_QUATERNION_DATA_Y_MSB_ADDR                       = 0X25,
        BNO055_QUATERNION_DATA_Z_LSB_ADDR                       = 0X26,
        BNO055_QUATERNION_DATA_Z_MSB_ADDR                       = 0X27,

        /* Linear acceleration data registers */
        BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR                     = 0X28,
        BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR                     = 0X29,
        BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR                     = 0X2A,
        BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR                     = 0X2B,
        BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR                     = 0X2C,
        BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR                     = 0X2D,

        /* Gravity data registers */
        BNO055_GRAVITY_DATA_X_LSB_ADDR                          = 0X2E,
        BNO055_GRAVITY_DATA_X_MSB_ADDR                          = 0X2F,
        BNO055_GRAVITY_DATA_Y_LSB_ADDR                          = 0X30,
        BNO055_GRAVITY_DATA_Y_MSB_ADDR                          = 0X31,
        BNO055_GRAVITY_DATA_Z_LSB_ADDR                          = 0X32,
        BNO055_GRAVITY_DATA_Z_MSB_ADDR                          = 0X33,

        /* Temperature data register */
        BNO055_TEMP_ADDR                                        = 0X34,

        /* Status registers */
        BNO055_CALIB_STAT_ADDR                                  = 0X35,
        BNO055_SELFTEST_RESULT_ADDR                             = 0X36,
        BNO055_INTR_STAT_ADDR                                   = 0X37,

        BNO055_SYS_CLK_STAT_ADDR                                = 0X38,
        BNO055_SYS_STAT_ADDR                                    = 0X39,
        BNO055_SYS_ERR_ADDR                                     = 0X3A,

        /* Unit selection register */
        BNO055_UNIT_SEL_ADDR                                    = 0X3B,
        BNO055_DATA_SELECT_ADDR                                 = 0X3C,

        /* Mode registers */
        BNO055_OPR_MODE_ADDR                                    = 0X3D,
        BNO055_PWR_MODE_ADDR                                    = 0X3E,

        BNO055_SYS_TRIGGER_ADDR                                 = 0X3F,
        BNO055_TEMP_SOURCE_ADDR                                 = 0X40,

        /* Axis remap registers */
        BNO055_AXIS_MAP_CONFIG_ADDR                             = 0X41,
        BNO055_AXIS_MAP_SIGN_ADDR                               = 0X42,

        /* SIC registers */
        BNO055_SIC_MATRIX_0_LSB_ADDR                            = 0X43,
        BNO055_SIC_MATRIX_0_MSB_ADDR                            = 0X44,
        BNO055_SIC_MATRIX_1_LSB_ADDR                            = 0X45,
        BNO055_SIC_MATRIX_1_MSB_ADDR                            = 0X46,
        BNO055_SIC_MATRIX_2_LSB_ADDR                            = 0X47,
        BNO055_SIC_MATRIX_2_MSB_ADDR                            = 0X48,
        BNO055_SIC_MATRIX_3_LSB_ADDR                            = 0X49,
        BNO055_SIC_MATRIX_3_MSB_ADDR                            = 0X4A,
        BNO055_SIC_MATRIX_4_LSB_ADDR                            = 0X4B,
        BNO055_SIC_MATRIX_4_MSB_ADDR                            = 0X4C,
        BNO055_SIC_MATRIX_5_LSB_ADDR                            = 0X4D,
        BNO055_SIC_MATRIX_5_MSB_ADDR                            = 0X4E,
        BNO055_SIC_MATRIX_6_LSB_ADDR                            = 0X4F,
        BNO055_SIC_MATRIX_6_MSB_ADDR                            = 0X50,
        BNO055_SIC_MATRIX_7_LSB_ADDR                            = 0X51,
        BNO055_SIC_MATRIX_7_MSB_ADDR                            = 0X52,
        BNO055_SIC_MATRIX_8_LSB_ADDR                            = 0X53,
        BNO055_SIC_MATRIX_8_MSB_ADDR                            = 0X54,

        /* Accelerometer Offset registers */
        ACCEL_OFFSET_X_LSB_ADDR                                 = 0X55,
        ACCEL_OFFSET_X_MSB_ADDR                                 = 0X56,
        ACCEL_OFFSET_Y_LSB_ADDR                                 = 0X57,
        ACCEL_OFFSET_Y_MSB_ADDR                                 = 0X58,
        ACCEL_OFFSET_Z_LSB_ADDR                                 = 0X59,
        ACCEL_OFFSET_Z_MSB_ADDR                                 = 0X5A,

        /* Magnetometer Offset registers */
        MAG_OFFSET_X_LSB_ADDR                                   = 0X5B,
        MAG_OFFSET_X_MSB_ADDR                                   = 0X5C,
        MAG_OFFSET_Y_LSB_ADDR                                   = 0X5D,
        MAG_OFFSET_Y_MSB_ADDR                                   = 0X5E,
        MAG_OFFSET_Z_LSB_ADDR                                   = 0X5F,
        MAG_OFFSET_Z_MSB_ADDR                                   = 0X60,

        /* Gyroscope Offset register s*/
        GYRO_OFFSET_X_LSB_ADDR                                  = 0X61,
        GYRO_OFFSET_X_MSB_ADDR                                  = 0X62,
        GYRO_OFFSET_Y_LSB_ADDR                                  = 0X63,
        GYRO_OFFSET_Y_MSB_ADDR                                  = 0X64,
        GYRO_OFFSET_Z_LSB_ADDR                                  = 0X65,
        GYRO_OFFSET_Z_MSB_ADDR                                  = 0X66,

        /* Radius registers */
        ACCEL_RADIUS_LSB_ADDR                                   = 0X67,
        ACCEL_RADIUS_MSB_ADDR                                   = 0X68,
        MAG_RADIUS_LSB_ADDR                                     = 0X69,
        MAG_RADIUS_MSB_ADDR                                     = 0X6A
    } reg_t;

    typedef enum
    {
        POWER_MODE_NORMAL                                       = 0X00,
        POWER_MODE_LOWPOWER                                     = 0X01,
        POWER_MODE_SUSPEND                                      = 0X02
    } powermode_t;

    typedef enum
    {
        /* Operation mode settings*/
        OPERATION_MODE_CONFIG                                   = 0X00,
        OPERATION_MODE_ACCONLY                                  = 0X01,
        OPERATION_MODE_MAGONLY                                  = 0X02,
        OPERATION_MODE_GYRONLY                                  = 0X03,
        OPERATION_MODE_ACCMAG                                   = 0X04,
        OPERATION_MODE_ACCGYRO                                  = 0X05,
        OPERATION_MODE_MAGGYRO                                  = 0X06,
        OPERATION_MODE_AMG                                      = 0X07,
        OPERATION_MODE_IMUPLUS                                  = 0X08,
        OPERATION_MODE_COMPASS                                  = 0X09,
        OPERATION_MODE_M4G                                      = 0X0A,
        OPERATION_MODE_NDOF_FMC_OFF                             = 0X0B,
        OPERATION_MODE_NDOF                                     = 0X0C
    } opmode_t;

    typedef enum
    {
        REMAP_CONFIG_P0                                         = 0x21,
        REMAP_CONFIG_P1                                         = 0x24, // default
        REMAP_CONFIG_P2                                         = 0x24,
        REMAP_CONFIG_P3                                         = 0x21,
        REMAP_CONFIG_P4                                         = 0x24,
        REMAP_CONFIG_P5                                         = 0x21,
        REMAP_CONFIG_P6                                         = 0x21,
        REMAP_CONFIG_P7                                         = 0x24
    } axis_remap_config_t;

    typedef enum
    {
        REMAP_SIGN_P0                                           = 0x04,
        REMAP_SIGN_P1                                           = 0x00, // default
        REMAP_SIGN_P2                                           = 0x06,
        REMAP_SIGN_P3                                           = 0x02,
        REMAP_SIGN_P4                                           = 0x03,
        REMAP_SIGN_P5                                           = 0x01,
        REMAP_SIGN_P6                                           = 0x07,
        REMAP_SIGN_P7                                           = 0x05
    } axis_remap_sign_t;

    typedef struct
    {
        uint8_t  accel_rev;
        uint8_t  mag_rev;
        uint8_t  gyro_rev;
        uint16_t sw_rev;
        uint8_t  bl_rev;
    } rev_info_t;

    typedef enum
    {
        VECTOR_ACCELEROMETER = BNO055_ACCEL_DATA_X_LSB_ADDR,
        VECTOR_MAGNETOMETER  = BNO055_MAG_DATA_X_LSB_ADDR,
        VECTOR_GYROSCOPE     = BNO055_GYRO_DATA_X_LSB_ADDR,
        VECTOR_EULER         = BNO055_EULER_H_LSB_ADDR,
        VECTOR_LINEARACCEL   = BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR,
        VECTOR_GRAVITY       = BNO055_GRAVITY_DATA_X_LSB_ADDR
    } vector_type_t;

    typedef struct
    {
        uint16_t accel_offset_x;
        uint16_t accel_offset_y;
        uint16_t accel_offset_z;
        uint16_t gyro_offset_x;
        uint16_t gyro_offset_y;
        uint16_t gyro_offset_z;
        uint16_t mag_offset_x;
        uint16_t mag_offset_y;
        uint16_t mag_offset_z;

        uint16_t accel_radius;
        uint16_t mag_radius;
    } calibration_offsets_t;


    /// Raw vector results.  
    typedef Driver::Imu::Vector<int16_t> raw_vector_t;

    /// vector results in engineering units.  
    typedef Driver::Imu::Vector<float> vector_t;
    
    /// Raw quat results.  
    typedef Driver::Imu::Quat<int16_t> raw_quat_t;

    /// vector results in engineering units.  
    typedef Driver::Imu::Quat<float> quat_t;
    



#if defined (ARDUINO_SAMD_ZERO) && ! (ARDUINO_SAMD_FEATHER_M0)
#error "On an arduino Zero, BNO055's ADR pin must be high. Fix that, then delete this line."
    /// Constructor
    Adafruit( int32_t sensorID = -1, uint8_t address = DRIVER_IMU_BNO005_ADAFRUIT_ADDRESS_B );
#else
    /// Constructor
    Adafruit( int32_t sensorID = -1, uint8_t address = DRIVER_IMU_BNO005_ADAFRUIT_ADDRESS_A );
#endif

public:
    /// Initializes the BNO055 Chip and starts the driver
    bool  start( opmode_t mode = OPERATION_MODE_NDOF );

    /// Puts the chip in the specified operating mode
    void  setMode( opmode_t mode );

    /// Gets the chip's revision numbers
    void  getRevInfo( rev_info_t* );

    /// Use the external 32.768KHz crystal
    void  setExtCrystalUse( bool usextal );

    /** Gets the latest system status info.

        System Status (see section 4.3.58)
           ---------------------------------
           0 = Idle
           1 = System Error
           2 = Initializing Peripherals
           3 = System Initialization
           4 = Executing Self-Test
           5 = Sensor fusio algorithm running
           6 = System running without fusion algorithms


        Self Test Results (see section )
        --------------------------------
        1 = test passed, 0 = test failed

        Bit 0 = Accelerometer self test
        Bit 1 = Magnetometer self test
        Bit 2 = Gyroscope self test
        Bit 3 = MCU self test

        0x0F = all good!

        System Error (see section 4.3.59)
        ---------------------------------
        0 = No error
        1 = Peripheral initialization error
        2 = System initialization error
        3 = Self test result failed
        4 = Register map value out of range
        5 = Register map address out of range
        6 = Register map write error
        7 = BNO low power mode not available for selected operation mode
        8 = Accelerometer power mode not available
        9 = Fusion algorithm configuration error
        A = Sensor configuration error
     */
    void  getSystemStatus( uint8_t *system_status, uint8_t *self_test_result, uint8_t *system_error );

    /** Gets current calibration state.  Each value should be a uint8_t
        pointer and it will be set to 0 if not calibrated and 3 if
        fully calibrated.
     */
    void  getCalibration( uint8_t* system, uint8_t* gyro, uint8_t* accel, uint8_t* mag );


public:
    /// Gets a vector reading - in engineering units - from the specified source.
    vector_t getVector( vector_type_t vector_type ) { return convert( vector_type, getRawVector( vector_type ) ); }

    /// Gets a vector reading - in raw IMU counts - from the specified source.
    raw_vector_t getRawVector( vector_type_t vector_type );

    /// Converts a raw vector to a engineering unit's vector
    vector_t convert( vector_type_t vector_type, raw_vector_t raw_vector );


public:
    /// Gets a quaternion reading - in engineering units - from the specified source
    quat_t getQuat( void ) { return convert( getRawQuat() ); }

    /// Gets a quaternion reading - in raw IMU counts - from the specified source
    raw_quat_t getRawQuat( void );

    /// Converts a raw quat to a engineering unit's quat
    quat_t convert( raw_quat_t raw_quat );


public:
    /// Gets the temperature in degrees Celsius
    int8_t getTemp( void );


public:
    /** Reads the sensor's offset registers into a byte array.  If the
        IMU is NOT in the fully calibrated state this method returns
        false and 'calibData' is NOT updated.
     */
    bool  getSensorOffsets( uint8_t* calibData );

    /** Reads the sensor's offset registers into an offset struct. If the
        IMU is NOT in the fully calibrated state this method returns
        false and 'calibData' is NOT updated.
     */
    bool  getSensorOffsets( calibration_offsets_t &offsets_type );

    /// Writes an array of calibration values to the sensor's offset registers
    void  setSensorOffsets( const uint8_t* calibData );

    /// Writes to the sensor's offset registers from an offset struct
    void  setSensorOffsets( const calibration_offsets_t &offsets_type );

    /// Returns true if the IMU is fully calibrated
    bool  isFullyCalibrated( void );


protected:
    /// Reads an 8 bit value over I2C
    uint8_t  read8( reg_t );

    /// Reads the specified number of bytes over I2C
    bool  readLen( reg_t, uint8_t* buffer, uint8_t len );

    /// Writes an 8 bit value over I2C
    bool  write8( reg_t, uint8_t value );

protected:
    /// I2C Address of the BNO055 chip
    uint8_t _address;

    /// Expected Sensor ID for the BNO055 chip
    int32_t _sensorID;

    /// Current 'user' selected operation mode of the BNO055 chip
    opmode_t _mode;
};


};      // end Namespaces
};
};

#endif // end DOXYGEN_WILL_SKIP_THIS

#endif  // end Header latch
