#pragma once
#include "rgb_matrix_types.h"
#include "color.h"

class RgbMatrix {
    
    private:
        RGB *pinValues;
        uint8_t numLEDs;
        uint8_t maxBrightness;

        void put_pixel(RGB c);
        RGB limitBrightness (RGB c);
    public:
        RgbMatrix();
        void setAll(RGB c);
        void set(uint8_t n, RGB c);
        void draw();
        void initializeRGB(uint8_t ledPin, uint8_t numLED, bool isRGBW, uint8_t maxbrightness);
}; 