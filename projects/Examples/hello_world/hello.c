/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init( LED_PIN );
    gpio_set_dir( LED_PIN, GPIO_OUT );

    stdio_init_all();
    int ledState = 1;
    while (true) {
        printf("Hello, world!\n");
        gpio_put( LED_PIN, ledState );
        sleep_ms(1000);
        ledState ^= 1;
    }
    return 0;
}
