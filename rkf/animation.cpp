#include "animation.h"
#include "pico/stdlib.h"
#include "rgb_matrix_types.h"
#include "config_rgb.h"


//
// Breathe
//
uint8_t breatheBrightness = 0;
int8_t breatheDirection = 1;
void animateBreathe(RgbMatrix *rgbMatrix) {
    breatheBrightness = breatheBrightness + breatheDirection;
    if (breatheBrightness >= 255) breatheDirection = -1;
    if (breatheBrightness <= 0) breatheDirection = 1;

    for (uint8_t i=0; i<RGBLED_NUM; i++) {
        uint8_t xGradient = g_led_config.point[i].x * 1.13839; // Scale to 255
        uint8_t yGradient = g_led_config.point[i].y * 3.98;    // Scale to 255

        xGradient = (xGradient * breatheBrightness) / 255;

        RGB c = {
            xGradient,
            (uint8_t)(breatheBrightness-xGradient),
            0
        };

        rgbMatrix->set(i, c);
    }
}