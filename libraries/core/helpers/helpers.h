#include "pico/stdlib.h"
#include <stdio.h>

#define DISABLE_INTERUPTS uint32_t interuptStates = save_and_disable_interrupts();
#define ENABLE_INTERUPTS restore_interrupts(interuptStates);

void serialLog (uint32_t output);
void printBits(size_t const size, void const * const ptr);
void printString(uint8_t data[], uint8_t size);
uint8_t lerp255(uint8_t value, uint8_t new_max);
void intArrayToCharArray(uint8_t *i[], char *c[]);

