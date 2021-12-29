#include "pico/stdlib.h"
#include <stdio.h>

void serialLog (uint32_t output);
void printBits(size_t const size, void const * const ptr);
uint8_t lerp255(uint8_t value, uint8_t new_max);
