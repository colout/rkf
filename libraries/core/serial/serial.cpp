#include <stdio.h>
#include "config.h"
#include "pico/stdlib.h"
#include "serial.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "serial.pio.h"


#define SERIAL_DELAY 400
#define CLOCK_FREQ 2000

bool serialState = true;

//
// Helpers
//
// static inline void serialDebug() {
//     #define ENABLE_SERIAL_DEBUG
//     #ifdef ENABLE_SERIAL_DEBUG
//         serialState=!serialState;
//         if (serialState) gpio_pull_up(DEBUG_PIN);
//         if (!serialState) gpio_pull_down(DEBUG_PIN);
//     #endif
// }

static inline void high() {
    gpio_put(DATA_PIN, true);
}

static inline void low() {
    gpio_put(DATA_PIN, false);
}

static inline void delayHalf() {
    busy_wait_us_32(SERIAL_DELAY / 2);
    
    //Nops<500>::generate();
}

static inline void delayFull() {
    busy_wait_us_32(SERIAL_DELAY);
    
    //delayHalf();delayHalf();
}

static inline bool read() {
    return gpio_get(DATA_PIN);  
}

static inline void modeWrite() {
    gpio_set_dir(DATA_PIN, GPIO_OUT);  
}

static inline void modeRead() {
    gpio_set_dir(DATA_PIN, GPIO_IN);
}

static inline void pullDown() {
    gpio_pull_down(DATA_PIN);
}

static inline void pullUp() {
    gpio_pull_up(DATA_PIN);
}

static inline void pullFloat() {
    gpio_disable_pulls(DATA_PIN);
}


void serialInitSettings(PIO pio, uint sm, uint pin, uint debug_pin, float pio_freq, bool isLeader) {
    pio_sm_config c;
    uint offset;

    float div = (float)clock_get_hz(clk_sys) / pio_freq;
 

    switch (isLeader) {
        case true:
            offset = pio_add_program(pio, &serialLeader_program);
            c = serialLeader_program_get_default_config(offset);
            break;
        case false:
            offset = pio_add_program(pio, &serialFollower_program);
            c = serialFollower_program_get_default_config(offset);
            break;
    }
   
    // Serial pin
    gpio_pull_up(pin);
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);
    sm_config_set_out_pins(&c, pin, 1);
    sm_config_set_set_pins(&c, pin, 1);
    sm_config_set_in_pins(&c, pin);

    // FOR DEBUG
    gpio_pull_up(debug_pin);
    pio_gpio_init(pio, debug_pin);
    pio_sm_set_consecutive_pindirs(pio, sm, debug_pin, 1, true);
    sm_config_set_sideset_pins(&c, debug_pin);   // FOR DEBUG

    // Shift Register Config
    sm_config_set_in_shift(&c, true, false, 8);
    sm_config_set_out_shift(&c, true, false, 8);
    
    // Clock config
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);

    pio_sm_set_enabled(pio, sm, true);
}




void serialLeaderInit() {
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    serialInitSettings(pio, sm, DATA_PIN, DEBUG_PIN, CLOCK_FREQ, true);

    // Do nothing
    while (true) {
        sleep_ms(100);

        pio_sm_put_blocking(pio, sm, 1);
        pio_sm_put_blocking(pio, sm, 170);
    }
}

void serialFollowerInit() {
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    serialInitSettings(pio, sm, DATA_PIN, DEBUG_PIN, CLOCK_FREQ, false);

    // Do nothing
    while (true) {
        uint8_t data = pio_sm_get_blocking(pio, sm) >> 24;

        printf ("\nData: %d\n", data);
    }
}