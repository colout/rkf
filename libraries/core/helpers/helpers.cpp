#include "helpers.h"
#include "pico/stdlib.h"
#include <stdio.h>

void serialLog (uint32_t output) {
    char outText[50] = {0};
    sprintf (outText, "%d", output);
    printf (outText);
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}

uint8_t lerp255(uint8_t value, uint8_t new_max) {
    // Scale down from 255 to new range

    uint32_t v = (uint32_t)(new_max * value) / 255;
    return v;
}