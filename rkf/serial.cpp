#include "pico/stdlib.h"
#include "config.h"
#include "serial.h"
#include "helpers.h"
#include "hardware/sync.h"


 // For troubleshootingg / testing
#include "rgb_matrix.h"


// Parity
#define ODD_PARITY 1
#define EVEN_PARITY 0
#define PARITY EVEN_PARITY

// 6us per bit == 166.67 kbits/s ==  166.67 bits/ms
// assuming 12 bits per byte (including overhead), 13.89 bytes/ms
// There is also ready-setup time and direction switch overhead, which should be less than a byte each:
//  TODO: add accurate calculation here
#define SERIAL_DELAY 6

bool serialState = 1;


// Private declarations
static volatile void serialSyncSend();
static volatile void serialSyncReceive();
static void readerReady();
static void writerReady();
static void switchToReader();
static void switchToWriter();
void sendCheckPacketInLoop();
bool waitForCheckPacket();

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



static inline volitile void high() {
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





static inline void serialDebug() {
#define ENABLE_SERIAL_DEBUG
#ifdef ENABLE_SERIAL_DEBUG
    serialState=!serialState;
    if (serialState) gpio_pull_up(DEBUG_PIN);
    if (!serialState) gpio_pull_down(DEBUG_PIN);
#endif
}

void serialLeaderInit() {
    //1) Leader read mode with pullup
    gpio_init(DATA_PIN);
    switchToWriter();

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);
    
    //while (read()) {} // leader will likely come online first, so follower will be floating.  wait until pulled down for first time
    
    serialDebug();  // first run will set to 0
}

static void switchToWriter() {
    modeRead();
    high();
    pullUp();
    delayFull();
}

void serialFollowerInit() {
    //1) Follower write mode pulled down
    gpio_init(DATA_PIN);
    switchToReader();

    gpio_init(DEBUG_PIN);
    gpio_set_dir(DEBUG_PIN, GPIO_IN);

    serialDebug();  // first run will set to 0
}

static void switchToReader() {
    modeWrite();
    pullFloat();
    low();
    delayFull();
}

//
// Writer
//
static bool leaderReady() {
    //2) Read mode while waiting for follower to release the wire from ground
    absolute_time_t readyWaitTimeoutTimer = make_timeout_time_us(16000);
    while (!read()) {
        if (time_reached(readyWaitTimeoutTimer)) {
            return 0;
        }
    }

    //2.5) Write mode before sync
    delayFull();    // For testng delay between follower and leader.  probably not necessary. Delete this

    writerReady();

    return 1;
}

static void writerReady() {

    modeWrite();
    pullFloat();
}


static volatile void serialSyncSend() {
    //4) Pull high for 1 delay low for 1 delay for reader to get timing from falling edge
    high();
    delayFull();
    low();
    delayFull();
}


static volatile void serialWriteByte(uint8_t data) {
    uint8_t b, p;
    const uint8_t bit = 8;
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

    high();  // Need high for sync

    delayFull();

}


static volatile void serialWritePacket(uint8_t *buffer, uint8_t size) {
    
    serialDebug();
    serialSyncSend();
    serialDebug();
    //Nops<100>::generate();
    busy_wait_us_32(4);
    serialWriteByte(size);
    

    for (uint8_t i = 0; i < size; ++i) {
        uint8_t data;
        data = buffer[i];
        serialSyncSend();
        serialWriteByte(data);
    } 
}

//
// Reader
//
static void followerReady() {
    readerReady();
}

static void readerReady() {
    //1) Ready, so release from ground

    modeRead();
    pullUp();
    //serialSyncReceive();
}

static volatile void serialSyncReceive(void) {
    //2) Wait for low pull's falling edge then wait to get timing right
    
    //First, wait a half delay OR pull high to account for potential slow skew
    absolute_time_t syncTimeoutTimer = make_timeout_time_us(SERIAL_DELAY / 2);
    while (!time_reached(syncTimeoutTimer) || !read()) {}

    // Wait for pull low
    syncTimeoutTimer = make_timeout_time_us(SERIAL_DELAY);
    while (!time_reached(syncTimeoutTimer) || read()) {}  // Timeout to avoid race conditions


    delayFull();
}

static volatile uint8_t serialReadByte()  {
    uint8_t byte, i, p, pb;
    const uint8_t bit = 8;

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

    delayHalf();

    //if (p != pb) return 0; // Zero when failed parity
    return byte;
}

static void serialReadPacket(uint8_t *buffer) {
    int size;  // Not sure why, but many comparisons fail (<,>,!=, etc) if uint8_t
    //serialDebug();
    serialSyncReceive();
    size = serialReadByte();

    //if (size != 2) size = 2;  

    for (int i = 0; i < size; ++i) {
        uint8_t data;
        serialSyncReceive();
        data = serialReadByte();
        buffer[i] = data;
    }
}












//
// Count test code
//

// TODO: This will be converted to an actual protocol
uint8_t a=0;

uint8_t this_num=0;
uint8_t last_num=0;

uint8_t dataSize = 2;
uint8_t sendData[2];
uint8_t revieveData[2] = {0};

bool isRedAlert = false;
uint64_t iterationCount = 0;


void countTest() {
    //printf(".");
    iterationCount++;
    absolute_time_t t = get_absolute_time();

    for (uint8_t i=0; i<dataSize; i++) {
        sendData[i] = i;
    }

    if (!IS_LEADER) {
        // Serial stuff.  Move to serial.cpp


        DISABLE_INTERUPTS;
        if (!leaderReady()) {
            switchToWriter();
            ENABLE_INTERUPTS;
            return;  // If takes too long to get ready, revert to good state and exit.
        }
        serialWritePacket(sendData, dataSize);

        readerReady();
        serialReadPacket(revieveData);
        switchToWriter();
        ENABLE_INTERUPTS;
    }
    
    if (!IS_FOLLOWER) {
        // Serial stuff


        DISABLE_INTERUPTS;
        followerReady();
        serialReadPacket(revieveData);
        delayFull(); // There is a delay at end off write, so read will end one tick behind here.  
        delayFull(); // Probably good to delay some more to give time for other side to get ready.  

        writerReady();
        delayFull(); // Probably good to delay.  
        serialWritePacket(sendData, dataSize);
        switchToReader();
        ENABLE_INTERUPTS;
        
    }
    //printf(" %jd ",  absolute_time_diff_us(t, get_absolute_time()));

    // The check
    for (uint8_t i=0; i<dataSize; i++) {
        if (revieveData[i] != sendData[i]) {
            //printf ("Failed at %d.  Expected %d recieved %d\n", i, sendData[i], revieveData[i]);
            isAlertState=true;
            printf("-");
        } //else printf("+");
    }
}