#include <stdio.h>
#include <string.h>

#include "matrix_scan.h"
#include "config.h"
#include "helpers.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "process_record.h"

static uint32_t row_scanmask;

const uint8_t row_pins[] = MATRIX_ROW_PINS;
const uint8_t col_pins[] = MATRIX_COL_PINS;

static uint32_t row_scanval[MATRIX_ROWS];
static uint32_t col_masks[MATRIX_COLS];


typedef struct {
    bool on;
    uint8_t last;
} scan_state;

static scan_state matrix_states[MATRIX_ROWS * MATRIX_COLS];

void matrixInit () {

    for (int i = 0; i < MATRIX_COLS; i++) {
        uint8_t io = col_pins[i];
        gpio_init(io);
        gpio_set_dir(io, GPIO_IN);
        gpio_pull_up(io);
        col_masks[i] = 1ul << io;
    }

    uint32_t mask = 0;
    for (int i = 0; i < MATRIX_ROWS; i++) {
        uint8_t io = row_pins[i];
        gpio_init(io);
        gpio_set_dir(io, GPIO_OUT);
        gpio_disable_pulls(io);
        row_scanval[i] = ~(1ul << io);
        mask |= 1ul << io;
    }

    row_scanmask = mask;
    
    for (int i = 0; i < MATRIX_ROWS; i++) {
        row_scanval[i] &= mask;
    }

    memset(matrix_states, 0, sizeof(matrix_states));
}

__attribute__((weak)) void matrix_changed(uint ncol, uint nrow, bool on, uint8_t when) {
    printf("matrix_changed: col=%d row=%d %s when=%d\n", ncol, nrow, on ? "ON" : "OFF", when);
}

__attribute__((weak)) void matrix_suppressed(uint ncol, uint nrow, bool on, uint8_t when, uint8_t last, uint8_t elapsed) {
    //printf("matrix_suppressed: col=%d row=%d %s when=%d last=%d elapsed=%d\n", ncol, nrow, on ? "ON" : "OFF", when, last, elapsed);
}

void matrixScan() {
    static uint8_t count = 0;
    uint x = 0;

    //printf("\nMatrix scan\n");
    for (uint nrow = 0; nrow < MATRIX_ROWS; nrow++) {
        gpio_put_masked(row_scanmask, row_scanval[nrow]);
        sleep_us(SCAN_INTERVAL_US);
        uint32_t bits = gpio_get_all();
        for (uint ncol = 0; ncol < MATRIX_COLS; ncol++) {
            bool on = (bits & col_masks[ncol]) == 0;
            if (on != matrix_states[x].on) {
                uint8_t elapsed = count - matrix_states[x].last;

                if (elapsed >= DEBOUNCE_COUNT) {
                    matrix_states[x].on = on;
                    matrix_states[x].last = count;
                    //matrix_changed(ncol, nrow, on, count);

                    keyrecord_t record;

                    record.event.key.col = ncol;
                    record.event.key.row = nrow;
                    record.event.pressed = on;
                    record.event.time = to_ms_since_boot(get_absolute_time());
                    record.keycode = KC_UNDEFINED;

                    process_record(&record);

                } else {} // Debounced
            }
            x++;
        }
    }
    count++;
}