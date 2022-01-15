#include "pico/stdlib.h"

void serialDebug();
static inline void modeWrite();


void serialLeaderInit();
void serialFollowerInit();

// Sender
void leaderReady();
void serialSyncSend();
void serialWriteByte(uint8_t data, uint8_t bit);

// Receiver
void followerReady();
void serialSyncReceive();
uint8_t serialReadByte(uint8_t bit);
