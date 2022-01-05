#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "serial_1wire.pio.h"

void serialWriteBytes(uint sm, uint8_t bytes[], int len);
void serialReadBytes(uint sm, uint8_t bytes[], int len);
uint serial_1wireInitalize(PIO p, int gpio);