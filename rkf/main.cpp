// sdk/std Libraries
#include <stdio.h>
#include "pico/stdlib.h"
#include "boards/sparkfun_promicro.h"

// Core Libraries
#include "rgb_matrix.h"
#include "helpers.h"


// Local stuff
#include "config.h"
#include "matrix_scan.h"
#include "hid.h"
#include "animation.h"
#include "serial.h"

// USB
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#define TIME_BETWEEN_ANIMATE_RGB 16

// Global (defined in config.h as extern)
bool IS_USB_CONNECTED;  // Detect USB
bool IS_LEADER;         // aka "Master"
bool IS_FOLLOWER;       // aka "Slave"

int main() {

    // Init USB and low level stuff first
    board_init();
    tusb_init();
    stdio_init_all();
    
    RgbMatrix rgbMatrix; 

    // Check if USB connected for 1 second to detect side
    absolute_time_t timerUSBInitEndTime = make_timeout_time_ms(500);
    while (!time_reached(timerUSBInitEndTime)) tud_task();
    IS_USB_CONNECTED = tud_connected();

    IS_LEADER = IS_USB_CONNECTED;
    IS_FOLLOWER = !IS_USB_CONNECTED;

    printf ("Program starting...\n");

    matrixInit();
    
    printf ("Initializing RGB...\n");
    rgbMatrix.initializeRGB(LED_PIN, RGBLED_NUM, false, RGBLIGHT_LIMIT_VAL);
    sleep_ms(100);
    
    printf ("Starting main loop...\n");

    bool drawLEDsNextFrame = false;

    absolute_time_t timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);

    while (true) {
        matrixScan();

        if (IS_LEADER) {
            hid_task();
            tud_task();
        }

        // Generate Animation
        if (time_reached(timerAnimateRGB)) {
            animateBreathe(&rgbMatrix);
            timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);
            drawLEDsNextFrame = true;

        // Draw animation
        } else if (drawLEDsNextFrame) {
            rgbMatrix.draw();
            drawLEDsNextFrame = false;
        }
    }
    return 0;
}
