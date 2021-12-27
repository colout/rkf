#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "helpers.h"
#include "config.h"

static const uint read_pins[] = MATRIX_ROW_PINS ;
static const uint write_pins[] = MATRIX_COL_PINS ;
static const uint read_num = MATRIX_ROWS;
static const uint write_num = MATRIX_COLS;

// static const uint read_pins[] =  MATRIX_COL_PINS;
// static const uint write_pins[] = MATRIX_ROW_PINS ;
// static const uint read_num = MATRIX_COLS;
// static const uint write_num =MATRIX_ROWS;

static const uint16_t scan_interval = 30;
static const uint16_t debounce = 5;

// matrix_task scan whole switch matrix.
void matrixScan() {
    for (uint nwrite = 0; nwrite < write_num; nwrite++) {\

        // pull to ground
        gpio_set_dir(write_pins[write_num], GPIO_OUT);
        gpio_put(write_pins[write_num], 0);

        sleep_us(scan_interval); 

        for (uint nread = 0; nread < read_num; nread++) {
            if (!gpio_get(read_pins[nread])) printf("presed");
        }

        // Float
        gpio_put(write_pins[write_num], 1);
        gpio_set_dir(write_pins[write_num], GPIO_IN);
    }
}

void matrixInit() {
    // setup pins of read.
    for (int i = 0; i < read_num; i++) {
        uint io = read_pins[i];
        gpio_init(io);
        gpio_set_dir(io, GPIO_IN);
        gpio_pull_up(io);
    }
    // setup pins of write.
    for (int i = 0; i < write_num; i++) {
        uint io = write_pins[i];
        gpio_init(io);
        gpio_set_dir(io, GPIO_OUT);
        gpio_pull_up(io);
        gpio_put(io, 1);
    }
}