#include "pico/stdlib.h"
#include "config.h"
#include "serial.h"
#include "helpers.h"
#include "hardware/sync.h"

// Quick code  for inerrupt handling


#define ODD_PARITY 1
#define EVEN_PARITY 0
#define PARITY EVEN_PARITY

// 6 when using serial debug. 4 if not (experimental)
#define SERIAL_DELAY 6

bool serialState = 1;


// Private declarations
static inline void serialSyncSend();
static inline void serialSyncReceive();

// Accurate Delay.
//  Usage: 
//  * Each noop delays for 8ns
//  * Nops<100>::generate();  // Delays for 800ns
template< unsigned N > struct Nops{
  static void generate() __attribute__((always_inline)){
    __asm volatile ("nop":);
    Nops< N - 1 >::generate();
  }
};
template<> struct Nops<0>{ static inline void generate(){} };



static inline void high() {
    gpio_put(DATA_PIN, true);
}

static inline void low() {
    gpio_put(DATA_PIN, false);
}

static inline void delayHalf() {
    busy_wait_us_32(SERIAL_DELAY / 2);
}

static inline void delayFull() {
    busy_wait_us_32(SERIAL_DELAY);
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





static inline void serialDebug() {
//#define ENABLE_SERIAL_DEBUG
#ifdef ENABLE_SERIAL_DEBUG
    serialState=!serialState;
    if (serialState) gpio_pull_up(DEBUG_PIN);
    if (!serialState) gpio_pull_down(DEBUG_PIN);
#endif
}

void serialLeaderInit() {
    //1) Leader read mode with pullup
    gpio_init(DATA_PIN);
    modeRead();
    high();
    pullUp();

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);
    
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

static inline void leaderReady() {
    //2) Read mode while waiting for follower to release the wire from ground
    while(!read()) {} //TODO: Timeout abort

    //2.5) Write mode before sync
    delayFull();    // For testng delay between follower and leader.  Not necessary. Delete this

    modeWrite();
    pullFloat();
    serialDebug();
    
    //3) Sync
    serialSyncSend();
}

static inline void serialSyncSend() {
    //4) Pull high for 1 delay low for 1 delay for reader to get timing from falling edge
    high();
    delayFull();
    low();
    delayFull();
}


static inline void serialWriteByte(uint8_t data, uint8_t bit) {
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
static inline void followerReady() {
    //1) Ready, so release from ground
    serialDebug();
    modeRead();
    pullUp();
    serialSyncReceive();
}

static inline void serialSyncReceive(void) {
    //2) Wait for low pull's falling edge then wait to get timing right
    
    //First, wait a half delay OR pull high to account for potential slow skew
    absolute_time_t syncTimeoutTimer = make_timeout_time_us(SERIAL_DELAY / 2);
    while (!time_reached(syncTimeoutTimer) || !read()) {}

    // Wait for pull low
    syncTimeoutTimer = make_timeout_time_us(SERIAL_DELAY);
    while (!time_reached(syncTimeoutTimer) || read()) {}

    serialDebug();
    delayFull();
}

static inline uint8_t serialReadByte(uint8_t bit)  {
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
    serialDebug();
    delayHalf();
    return byte;
}









//
// Count test code
//

// TODO: This will be converted to an actual protocol
uint8_t a=0;

uint8_t this_num=0;
uint8_t last_num=0;

void countTest() {
    if (!IS_LEADER) {

        //a=170;

        // Serial stuff.  Move to serial.cpp
        DISABLE_INTERUPTS;
        leaderReady();
        serialWriteByte(a, 8);
        ENABLE_INTERUPTS;
        serialLeaderInit();

        a++;

        sleep_us(1000);
    }
    if (!IS_FOLLOWER) {
        uint8_t c;
        //a=170;

        // Serial stuff
        DISABLE_INTERUPTS;
        followerReady();
        c=serialReadByte(8);
        ENABLE_INTERUPTS;
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