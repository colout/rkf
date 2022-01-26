#include "pico/stdlib.h"
#include "config.h"
#include "serial.h"
#include "helpers.h"
#include "hardware/pio.h"




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
    //1) Leader read mode with pullup
    gpio_init(DATA_PIN);
    modeRead();
    pullUp();

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);

    serialInitalize(pio0, 2);
    
    serialDebug();  // first run will set to 0
}

void serialFollowerInit() {
    //1) Follower write mode pulled down
    gpio_init(DATA_PIN);
    modeWrite();
    pullFloat();
    low();
    sleep_us(3000);

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);

    serialDebug();  // first run will set to 0
}

//
// Writer
//

void leaderReady() {

    //2) Read mode while waiting for follower to release the wire from ground
    modeRead();
    pullUp();
    while(!read()) {} //TODO: Timeout abort

    //2.5) Write mode before sync
    modeWrite();
    pullFloat();
    
    //3) Sync
    serialSyncSend();    
    
}

void serialSyncSend() {
    //4) Pull high for 1 delay low for 1 delay for reader to get timing from falling edge
    high();
    delayFull();
    low();
    delayFull();
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

    //low();  // sync_send() / senc_recv() need raise edge

    delayFull();
    //serialDebug();
}

//
// Reader
//
void followerReady() {
    //1) Ready, so release from ground
    modeRead();
    pullUp();
    serialDebug();
    
    //2) Wait for pull to ground
    
    
    serialSyncReceive();
}

void serialSyncReceive(void) {
    //4) Wait for low pull's falling edge then wait 1 delay to get timing right
    while (read()) {}
    delayFull();
    delayFull();
    
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


















uint8_t a=0;

uint8_t this_num=0;
uint8_t last_num=0;

void countTest() {
    if (!IS_LEADER) {

        // Serial stuff
        serialDebug();
        leaderReady();

        uint8_t sendBytes[] = {0xCC, 0x44};
        serialWriteBytes(sm, sendBytes, sizeof(sendBytes));

        //serialWriteByte(a, 8);
        //serialLeaderInit();

        a++;

        sleep_us(1000);
        serialDebug();
    }
    if (!IS_FOLLOWER) {
        // Serial stuff
        followerReady();
        uint8_t c=serialReadByte(8);
        serialFollowerInit();
        sleep_us(1000);

        if (c==0 && this_num==255 && last_num==254) {
            printf ("\ncount restarted.  re-syncing\n");
            a=0;
        }
        if (c!=a) {
            printf("error at %d. received %d\n", a, c);
            printf("bits: \n");
            printBits(sizeof(a), &a);
            printf(" vs \n");
            printBits(sizeof(c), &c);
            printf("\n");

            gpio_pull_up(D0);
            sleep_us(500);
        }
            gpio_pull_down(D0);
        last_num=this_num;
        this_num=c;
        a++;
        //printf ("%d\n", c);

    }
}