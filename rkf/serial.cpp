#include "pico/stdlib.h"
#include "config.h"
#include "serial.h"


#define ODD_PARITY 1
#define EVEN_PARITY 0
#define PARITY EVEN_PARITY

#define SERIAL_DELAY 400
#define DEBOUNCE_DELAY 10

bool serialState = 1;

// Private declarations
void serialSyncSend();
void serialSyncReceive();

static inline void high() {
    gpio_put(DATA_PIN, true);
}

static inline void low() {
    gpio_put(DATA_PIN, false);
}

static inline void delayFull() {
    sleep_us(SERIAL_DELAY);
}

static inline void delayHalf() {
    sleep_us(SERIAL_DELAY/2);    
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


void serialDebug() {
    serialState=!serialState;
    if (serialState) gpio_pull_up(DEBUG_PIN);
    if (!serialState) gpio_pull_down(DEBUG_PIN);
}

void serialLeaderInit() {
    gpio_init(DATA_PIN);
    modeWrite();
    gpio_disable_pulls(DATA_PIN);
    low();

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);
    
    serialDebug();  // first run will set to 0
}

void serialFollowerInit() {
    gpio_init(DATA_PIN);
    modeRead();
    gpio_pull_up(DATA_PIN);

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);

    serialDebug();  // first run will set to 0
}

//
// Writer
//

void writerReady() {

    //1) High means ready to send
    modeRead();
    pullUp();

    //2) Wait for reader to pull down for one full delay
    //sleep_us(DEBOUNCE_DELAY);
    while(read()) {}
    // for (uint8_t i = 0; i < SERIAL_DELAY * 100000; i++) {
    //     if (!read()) break;
    // }

    serialDebug();
    delayFull();
    
    //3) High for a delay
    modeWrite();
    pullFloat();
    high();
    delayFull();
    serialSyncSend();
}

void serialSyncSend() {
    //4) Pull low for one delay for reader to get timing
    low();
    delayFull();
    high();
}


void serialWriteByte(uint8_t data, uint8_t bit) {
    uint8_t b, p;
    for (p = PARITY, b = 1 << (bit - 1); b; b >>= 1) {
        if (data & b) {
            high();
            p ^= 1;
        } else {
            low();
            p ^= 0;
        }
        delayFull();
    }
    /* send parity bit */
    if (p & 1) {
        high();
    } else {
        low();
    }
    delayFull();

    low();  // sync_send() / senc_recv() need raise edge
    delayFull();
    serialDebug();
}

//
// Reader
//
void readerReady() {
    //sleep_us(DEBOUNCE_DELAY);

    while(!read()) {}  //1) wait for writer to float to high for ready  TODO: Timeout abort


    //sleep_us(DEBOUNCE_DELAY);
    
    
    //2) Immediately pull low for full delay to ack
    serialDebug();
    modeWrite();
    low();
    delayFull();
    serialDebug();
    
    //3) Float high.  Wait half delay to get ready for sync
    modeRead();
    pullUp();

    //sleep_us(DEBOUNCE_DELAY);
    
    serialSyncReceive();
}

void serialSyncReceive(void) {
    //4) Wait for low pull then wait 1 delay to get timing right
    for (uint8_t i = 0; i < SERIAL_DELAY * 50 && read(); i++) {
        // will exit if read==0 OR SERIAL_DELAY * 5 iterations
    }
    // This shouldn't hang if the target disconnects because the
    // serial line will float to high if the target does disconnect.
    while (!read()) {}
    
    //serialDebug();
}

uint8_t serialReadByte(uint8_t bit) {
    uint8_t byte, i, p, pb;

    for (i = 0, byte = 0, p = PARITY; i < bit; i++) {
        delayHalf();  // read the middle of pulses
        if (read()) {
            byte = (byte << 1) | 1;
            p ^= 1;
        } else {
            byte = (byte << 1) | 0;
            p ^= 0;
        }
        serialDebug();
        delayHalf();
    }
    /* recive parity bit */
    delayHalf();  // read the middle of pulses
    pb = read();
    //serialDebug();
    delayHalf();

    return byte;
}