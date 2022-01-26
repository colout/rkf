#include "pico/stdlib.h"
#include "hardware/pio.h"

void serialLeaderInit();
void serialFollowerInit();

void serialWriteBytes(uint sm, uint8_t bytes[], int len);
void serialReadBytes(uint sm, uint8_t bytes[], int len);
uint serialInitalize(PIO p, int gpio);
