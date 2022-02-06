#include <stdio.h>
#include "config.h"
#include "pico/stdlib.h"
#include "serial.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "serial.pio.h"


#define SERIAL_DELAY 400
#define CLOCK_FREQ 128000

bool serialState = true;

//
// Helpers

static inline void serialDebug() {
    #define ENABLE_SERIAL_DEBUG
    #ifdef ENABLE_SERIAL_DEBUG
        serialState=!serialState;
        if (serialState) gpio_pull_up(DEBUG_PIN);
        if (!serialState) gpio_pull_down(DEBUG_PIN);
    #endif
}

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


uint serialInitSettings(PIO pio, uint sm, uint pin, uint debug_pin, float pio_freq) {
    pio_sm_config c;
    uint offset;

    float div = (float)clock_get_hz(clk_sys) / pio_freq;
    
    //gpio_pull_up(pin);
    pio_gpio_init(pio, pin);

    offset = pio_add_program(pio, &serial_program);
    c = serial_program_get_default_config(offset);

    sm_config_set_out_shift(&c, true, false, 32);
    sm_config_set_in_shift(&c, true, false, 32);
    sm_config_set_sideset_pins(&c, pin);
   
    // Serial pin
    sm_config_set_out_pins(&c, pin, 1);
    sm_config_set_set_pins(&c, pin, 1);
    sm_config_set_in_pins(&c, pin);
    sm_config_set_jmp_pin(&c, pin);
    
    // Clock config
    sm_config_set_clkdiv(&c, div);

    // Start
    //if (!isLeader) while (read());  //  will pull down before starting. TODO: Timeout in case started much later
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    return offset;
}



static inline char uart_rx_program_getc(PIO pio, uint sm) {
    return pio_sm_get_blocking(pio, sm) >> 24;

    // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
    io_rw_8 *rxfifo_shift = (io_rw_8*)&pio->rxf[sm] + 3;
    while (pio_sm_is_rx_fifo_empty(pio, sm))
        tight_loop_contents();
    return (char)*rxfifo_shift;
}

static inline void serialWritePacket(PIO pio, uint sm, uint8_t *buffer, uint8_t size) {
    for (uint8_t i = 0; i < size; ++i) {
        uint8_t data;
        data = buffer[i];
        pio_sm_put_blocking(pio, sm, (uint32_t)data);
    }
}


static inline void serialReadPacket(PIO pio, uint sm, uint8_t *buffer, uint8_t size) {
    for (uint8_t i = 0; i < size; ++i) {
        uint8_t data;
        data = uart_rx_program_getc(pio, sm);
        buffer[i] = data;
        //serialDebug();
    }
}

static inline void serialReadMode(PIO pio, uint sm, uint offset) {
    uint serial_program_read_start = 1;    // Need to update this when PIO line numbers change
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_jmp(offset + serial_program_read_start));
}

static inline void serialWriteMode(PIO pio, uint sm, uint offset) {
    uint serial_program_write_start = 16;    // Need to update this when PIO line numbers change
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_jmp(offset + serial_program_write_start));
}


static inline void leaderNotReady(PIO pio, uint sm, uint pin, uint offset) {
    while (!pio_sm_is_tx_fifo_empty(pio, sm)) {}
    // Input with pullup
    busy_wait_ms(5);   // Would be nicee to detect when done
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);
    pullUp();

    // From soft serial POC
    // modeRead();
    // high();
    // pullUp();

    pio_sm_exec_wait_blocking(pio, sm, pio_encode_jmp(offset));
}

static inline void leaderReady(PIO pio, uint sm, uint pin, uint offset) {
    // Wait until writer is pulled high
    //pullFloat();
    busy_wait_ms(5);
    uint serial_program_leader_ready = 13;    // Need to update this when PIO line numbers change
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_jmp(offset + serial_program_leader_ready));

    //serialWriteMode(pio, sm, offset);
    
    // From soft serial POC
    // while(!read()) {} //TODO: Timeout abort
    // delayFull();
    // modeWrite();
    // pullFloat();
}

static inline void followerNotReady(PIO pio, uint sm, uint pin, int offset) {
    // Output and pull down
    //while (!pio_sm_is_rx_fifo_empty(pio, sm)) {}  // rx_fifo should always be empty because we pull blocking
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    //pio_sm_exec_wait_blocking(pio, sm, 0xe300);  // LOW on in
    low();

    // From soft serial POC
    //modeWrite();
    //pullFloat();
    //low();

    pio_sm_exec_wait_blocking(pio, sm, pio_encode_jmp(offset));  // Do noothing
}

static inline void followerReady(PIO pio, uint sm, uint pin, uint offset) {
    //pullUp();
    
    //high();
    //pio_sm_exec_wait_blocking(pio, sm, 0xe001);  // HIGH on pin
    serialReadMode(pio, sm, offset);

    
    // From soft serial POC
    // modeRead();
    // pullUp();
}



uint8_t dataSize = 255;
uint8_t sendData[255];
uint8_t revieveData[255] = {0};

void serialLeaderInit() {
    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);
    
    serialDebug();  
    sleep_ms(3000);
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = serialInitSettings(pio, sm, DATA_PIN, DEBUG_PIN, CLOCK_FREQ);

    leaderNotReady(pio, sm, DATA_PIN, offset);

    for (uint8_t i=0; i<dataSize; i++) {
        sendData[i] = i;
    }

    // Do nothing
    while (true) {
        //busy_wait_ms(5);
        serialDebug();  
        leaderReady(pio, sm, DATA_PIN, offset);
        serialWritePacket(pio, sm, sendData, dataSize);
        leaderNotReady(pio, sm, DATA_PIN, offset);
        serialDebug();  
    }
}

void serialFollowerInit() {
    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);
    
    serialDebug();  
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = serialInitSettings(pio, sm, DATA_PIN, DEBUG_PIN, CLOCK_FREQ);
    
    
    serialDebug();  
    followerNotReady(pio, sm, DATA_PIN, offset);

    for (uint8_t i=0; i<dataSize; i++) {
        sendData[i] = i;
    }

    // Do nothing
    while (true) {
        printf(".");
        // uint8_t data = pio_sm_get_blocking(pio, sm) >> 24;
        // printf ("\nData: %d\n", data);

        followerReady(pio, sm, DATA_PIN, offset);
        serialDebug();
        serialReadPacket(pio, sm, revieveData, dataSize);
        serialDebug();
        followerNotReady(pio, sm, DATA_PIN, offset);

        busy_wait_ms(10);

        for (uint8_t i=0; i<dataSize; i++) {
            if (revieveData[i] != sendData[i]) {
                printf ("Failed at %d.  Expected %d recieved %d\n", i, sendData[i], revieveData[i]);
            }
        }
    }
}