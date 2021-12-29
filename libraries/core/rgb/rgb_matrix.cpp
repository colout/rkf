#include "rgb_matrix_types.h"
/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "rgb_matrix.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "color.h"

void RgbMatrix::put_pixel(RGB c) {
    uint32_t rgb = rgbToInt32(c);
    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}

RGB RgbMatrix::limitBrightness (RGB c) {
    RGB v = {
        lerp255(c.r, maxBrightness),
        lerp255(c.g, maxBrightness),
        lerp255(c.b, maxBrightness)
    };

    return v;
}

void RgbMatrix::setAll(RGB c) {
    for (uint8_t i=0; i<numLEDs; i++) {
        set(i, c);
    }
}

void RgbMatrix::set(uint8_t n, RGB c) {
    pinValues[n] = c;
}

void RgbMatrix::draw() {
    for (uint8_t i=0; i<numLEDs; i++) {
        // Adjust to max brightness with lerp
        put_pixel(limitBrightness(pinValues[i]));
    }
}

RgbMatrix::RgbMatrix() {

}

void RgbMatrix::initializeRGB(uint ledPin, uint numleds, bool isRGBW, uint8_t maxbrightness) {
    numLEDs = numleds;
    pinValues = new RGB[numLEDs];
    maxBrightness = maxbrightness;

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, ledPin, 800000, isRGBW);
    
    RGB c = {0,0,0};
    setAll(c);
    draw();
}


/*
uint8_t brightness = 0;
int8_t breatheDirection = 1;
//auto ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, RGB_DI_PIN, RGBLED_NUM, PicoLed::FORMAT_GRB);

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void pattern_greys(uint len, uint t) {
    int max = 100; // let's not draw too much current!
    t %= max;
    for (int i = 0; i < len; ++i) {
        put_pixel(t * 0x10101);
        if (++t >= max) t = 0;
    }
}

void initializeRGB(uint8_t ledPin,  bool isRGBW) {
    //ledStrip.setBrightness(RGBLIGHT_LIMIT_VAL);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, ledPin, 800000, isRGBW);
    sleep_ms(1000);

    printf("Pattern out\n");

    int t = 0;
    while (1) {
        int pat = rand() % 1;
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        for (int i = 0; i < 1000; ++i) {
            pattern_greys(37, t);
            sleep_ms(10);
            t += dir;
        }
    }
}


void drawLEDs() {
    //ledStrip.show();
}

*/