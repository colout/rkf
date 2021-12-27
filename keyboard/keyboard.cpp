#include <stdio.h>
#include "config.h"

#include "pico/stdlib.h"
#include "rgb_matrix.h"
#include "helpers.h"

#include "keyboard.h"
#include "keymap.cpp"
#include "matrix_scan.h"

#define TIME_BETWEEN_ANIMATE_RGB 16

int main() {
    sleep_ms(2000);
    printf ("Program starting...\n");
    stdio_init_all();
    sleep_ms(100);
    //matrixInit();
    
    // printf ("Initializing RGB...\n");
    // initializeRGB();
    sleep_ms(100);
    
    printf ("Starting main loop...\n");

    bool drawLEDsNextFrame = false;

    absolute_time_t timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);

    while (true) {
        absolute_time_t timerFullLoop = get_absolute_time();
        
        //matrixScan();

        // Do non-matrix scan stuff.  Only do one per loop.
        if (time_reached(timerAnimateRGB)) {
            //animateBreathe();
            timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);
            drawLEDsNextFrame = true;
        } else if (drawLEDsNextFrame) {
            // Split the animation calculation and draw code between frames to get more matrix scans
            // drawLEDs();
            drawLEDsNextFrame = false;
        }

        ////Log loop time
        //serialLog(absolute_time_diff_us(timerFullLoop, get_absolute_time()));
        //printf (" main loop time\n");
    }
    sleep_ms(16);
    return 0;
}