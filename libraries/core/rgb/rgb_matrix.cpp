
#include <PicoLed.hpp>
#include "rgb_matrix_types.h"

uint8_t brightness = 0;
int8_t breatheDirection = 1;
auto ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, RGB_DI_PIN, RGBLED_NUM, PicoLed::FORMAT_GRB);

void initializeRGB() {
    ledStrip.setBrightness(RGBLIGHT_LIMIT_VAL);
}

void animateBreathe() {
    brightness = brightness + breatheDirection;
    if (brightness >= 255) breatheDirection = -1;
    if (brightness <= 0) breatheDirection = 1;

    for (int i=0; i<RGBLED_NUM; i++) {
        ledStrip.setPixelColor(i, PicoLed::RGB(brightness, 0, 0));
    }
}

void drawLEDs() {
    ledStrip.show();
}