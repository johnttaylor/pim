/*
 * Copyright (c) 2021, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "i2c_mux.h"
#include "scd4x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "sensirion_i2c_hal.h"
#include "sensirion_test_setup.h"
#include <inttypes.h>
#include <stdio.h>

TEST_GROUP (SCD4X_Tests) {
    void setup() {
        sensirion_i2c_hal_init();

        // Select MUX 2 channel 0
        int16_t error = sensirion_i2c_mux_set_single_channel(0x72, 0);
        CHECK_EQUAL_ZERO_TEXT(error, "sensirion_i2c_mux_set_single_channel");

        // try to reset sensor
        (void)scd4x_wake_up();
        (void)scd4x_stop_periodic_measurement();
        error = scd4x_reinit();
        CHECK_EQUAL_ZERO_TEXT(error, "scd4x_reinit");
    }

    void teardown() {
        sensirion_i2c_hal_free();
    }
};

TEST (SCD4X_Tests, SCD4X_Test_start_periodic_measurement) {
    int16_t error;
    error = scd4x_start_periodic_measurement();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_start_periodic_measurement");

    sensirion_i2c_hal_sleep_usec(5000000);
    uint16_t co2;
    uint16_t temperature;
    uint16_t humidity;
    error = scd4x_read_measurement_ticks(&co2, &temperature, &humidity);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_read_measurement_ticks");

    printf("co2: %i\n", co2);
    printf("temperature: %i\n", temperature);
    printf("humidity: %i\n", humidity);

    error = scd4x_stop_periodic_measurement();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_stop_periodic_measurement");
}

TEST (SCD4X_Tests, SCD4X_Test_read_measurement_ticks_fails_in_idle) {
    int16_t error;
    uint16_t co2;
    uint16_t temperature;
    uint16_t humidity;
    error = scd4x_read_measurement_ticks(&co2, &temperature, &humidity);
    CHECK_TEXT(error != 0, "scd4x_read_measurement_ticks should fail in idle");
}

TEST (SCD4X_Tests, SCD4X_Test_stop_periodic_measurement_in_idle) {
    int16_t error;
    error = scd4x_stop_periodic_measurement();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_stop_periodic_measurement");
}

TEST (SCD4X_Tests, SCD4X_Test_get_temperature_offset_ticks_after_reinit) {
    int16_t error;
    uint16_t t_offset;
    error = scd4x_get_temperature_offset_ticks(&t_offset);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_temperature_offset");
    printf("t_offset: %i\n", t_offset);

    CHECK_EQUAL_TEXT(t_offset, 1498, "T offset wrong after reinit")
}

TEST (SCD4X_Tests, SCD4X_Test_set_temperature_offset_ticks) {
    int16_t error;
    error = scd4x_set_temperature_offset_ticks(0);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_set_temperature_offset");

    uint16_t t_offset;
    error = scd4x_get_temperature_offset_ticks(&t_offset);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_temperature_offset");
    printf("t_offset: %i\n", t_offset);

    CHECK_EQUAL_TEXT(t_offset, 0, "T offset should be 0")
}

TEST (SCD4X_Tests, SCD4X_Test_get_sensor_altitude_default) {
    int16_t error;
    uint16_t sensor_altitude;
    error = scd4x_get_sensor_altitude(&sensor_altitude);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_sensor_altitude");
    printf("sensor_altitude: %i\n", sensor_altitude);

    CHECK_EQUAL_TEXT(sensor_altitude, 0,
                     "sensor_altitude should be 0 by default");
}

TEST (SCD4X_Tests, SCD4X_Test_set_sensor_altitude) {
    int16_t error;
    error = scd4x_set_sensor_altitude(42);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_set_sensor_altitude");

    uint16_t sensor_altitude;
    error = scd4x_get_sensor_altitude(&sensor_altitude);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_sensor_altitude");
    printf("sensor_altitude: %i\n", sensor_altitude);

    CHECK_EQUAL_TEXT(sensor_altitude, 42,
                     "sensor_altitude should be the set value");
}

TEST (SCD4X_Tests, SCD4X_Test_set_ambient_pressure) {
    int16_t error;
    uint16_t ambient_pressure = 0;
    error = scd4x_set_ambient_pressure(ambient_pressure);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_set_ambient_pressure");
}

TEST (SCD4X_Tests, SCD4X_Test_perform_forced_recalibration) {
    int16_t error;
    uint16_t target_co2_concentration = 400;
    uint16_t frc_correction;
    error = scd4x_perform_forced_recalibration(target_co2_concentration,
                                               &frc_correction);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_perform_forced_recalibration");
    printf("frc_correction: %i\n", frc_correction);
}

TEST (SCD4X_Tests, SCD4X_Test_get_automatic_self_calibration_default) {
    int16_t error;
    uint16_t asc_enabled;
    error = scd4x_get_automatic_self_calibration(&asc_enabled);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_automatic_self_calibration");
    printf("asc_enabled: %i\n", asc_enabled);
    CHECK_EQUAL_TEXT(asc_enabled, 1, "ASC should be enabled by default");
}

TEST (SCD4X_Tests, SCD4X_Test_set_automatic_self_calibration) {
    int16_t error;
    uint16_t asc_enabled = 0;
    error = scd4x_set_automatic_self_calibration(asc_enabled);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_set_automatic_self_calibration");

    error = scd4x_get_automatic_self_calibration(&asc_enabled);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_automatic_self_calibration");
    printf("asc_enabled: %i\n", asc_enabled);
    CHECK_EQUAL_TEXT(asc_enabled, 0, "ASC should be set correctly");
}

TEST (SCD4X_Tests, SCD4X_Test_start_low_power_periodic_measurement) {
    int16_t error;
    error = scd4x_start_low_power_periodic_measurement();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_start_low_power_periodic_measurement");

    sensirion_i2c_hal_sleep_usec(3000000);

    uint16_t co2;
    uint16_t temperature;
    uint16_t humidity;
    error = scd4x_read_measurement_ticks(&co2, &temperature, &humidity);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_read_measurement_ticks");

    printf("co2: %i\n", co2);
    printf("temperature: %i\n", temperature);
    printf("humidity: %i\n", humidity);

    error = scd4x_stop_periodic_measurement();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_stop_periodic_measurement");
}

TEST (SCD4X_Tests, SCD4X_Test_get_data_ready_status) {
    int16_t error;
    uint16_t data_ready;
    error = scd4x_get_data_ready_status(&data_ready);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_data_ready_status");
    printf("data_ready: %i\n", data_ready);
}

/*
 * Persist settings may break the test
TEST (SCD4X_Tests, SCD4X_Test_persist_settings) {
    int16_t error;
    error = scd4x_persist_settings();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_persist_settings");
}
*/

TEST (SCD4X_Tests, SCD4X_Test_get_serial_number) {
    int16_t error;
    uint16_t serial_0;
    uint16_t serial_1;
    uint16_t serial_2;
    error = scd4x_get_serial_number(&serial_0, &serial_1, &serial_2);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_get_serial_number");
    printf("serial_0: %i\n", serial_0);
    printf("serial_1: %i\n", serial_1);
    printf("serial_2: %i\n", serial_2);
}

TEST (SCD4X_Tests, SCD4X_Test_perform_self_test) {
    int16_t error;
    uint16_t sensor_status;
    error = scd4x_perform_self_test(&sensor_status);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_perform_self_test");
    printf("sensor_status: %i\n", sensor_status);
}

/*
 * Continous factory reset may break the device
TEST (SCD4X_Tests, SCD4X_Test_perform_factory_reset) {
    int16_t error;
    error = scd4x_perform_factory_reset();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_perform_factory_reset");
}
*/

TEST (SCD4X_Tests, SCD4X_Test_reinit) {
    int16_t error;
    error = scd4x_reinit();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_reinit");
}

TEST (SCD4X_Tests, SCD4X_Test_measure_single_shot) {
    int16_t error;
    error = scd4x_measure_single_shot();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_measure_single_shot");

    uint16_t co2;
    uint16_t temperature;
    uint16_t humidity;
    error = scd4x_read_measurement_ticks(&co2, &temperature, &humidity);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_read_measurement_ticks");

    printf("co2: %i\n", co2);
    printf("temperature: %i\n", temperature);
    printf("humidity: %i\n", humidity);
}

TEST (SCD4X_Tests, SCD4X_Test_measure_single_shot_rht_only) {
    int16_t error;
    error = scd4x_measure_single_shot_rht_only();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_measure_single_shot_rht_only");

    uint16_t co2;
    uint16_t temperature;
    uint16_t humidity;
    error = scd4x_read_measurement_ticks(&co2, &temperature, &humidity);
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_read_measurement_ticks");

    printf("co2: %i\n", co2);
    printf("temperature: %i\n", temperature);
    printf("humidity: %i\n", humidity);

    CHECK_EQUAL_TEXT(co2, 0, "CO2 should be zero in rht only measurement");
}

TEST (SCD4X_Tests, SCD4X_Test_power_down) {
    int16_t error;
    error = scd4x_power_down();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_power_down");

    sensirion_i2c_hal_sleep_usec(20000);

    error = scd4x_wake_up();
    printf("error: %i\n", error);
    CHECK_EQUAL_ZERO_TEXT(error, "wake-up should not forward error");
}

TEST (SCD4X_Tests, SCD4X_Test_wake_up) {
    int16_t error;
    error = scd4x_wake_up();
    CHECK_EQUAL_ZERO_TEXT(error, "scd4x_wake_up");
}
