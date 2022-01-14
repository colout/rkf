#include "pico/stdlib.h"

void serialDebug();

void serialLeaderInit();
void serialFollowerInit();

// Sender
void writerReady();
void serialSyncSend();
void serialWriteByte(uint8_t data, uint8_t bit);

// Receiver
void readerReady();
void serialSyncReceive();
uint8_t serialReadByte(uint8_t bit);