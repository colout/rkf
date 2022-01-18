// sdk/std Libraries
#include <stdio.h>
#include "pico/stdlib.h"
#include "boards/sparkfun_promicro.h"

// Core Libraries
#include "rgb_matrix.h"
#include "helpers.h"
#include "serial.h"


// Local stuff
#include "config.h"
#include "matrix_scan.h"

#ifndef DEBUG_MODE
    #include "hid.h"
#endif

#include "animation.h"

#ifndef DEBUG_MODE
    // USB
    #include "bsp/board.h"
    #include "tusb.h"
    #include "usb_descriptors.h"
#endif

#define TIME_BETWEEN_ANIMATE_RGB 16

// Global (defined in config.h as extern)
bool IS_USB_CONNECTED;  // Detect USB
bool IS_LEADER;         // aka "Master"
bool IS_FOLLOWER;       // aka "Slave"
uint serialSM;          // Serial state machine for 1wire

// For alert debugging
bool isAlertState = false;

int main() {
    

    // Init USB and low level stuff first
    #ifndef DEBUG_MODE
        board_init();
        tusb_init();
    #endif

    stdio_init_all();

    //uint sm = serial_1wireInitalize(pio0, 2);
    //uint8_t sendBytes[] = {0xCC, 0x44};
    //writeBytes(pio, sm, sendBytes, sizeof(sendBytes));
    //readBytes(pio, sm, data, 9);

    RgbMatrix rgbMatrix; 

    // Check if USB connected for 1 second to detect side
    #ifndef DEBUG_MODE
        absolute_time_t timerUSBInitEndTime = make_timeout_time_ms(500);
        while (!time_reached(timerUSBInitEndTime)) tud_task();
        IS_USB_CONNECTED = tud_connected();
    #else
        // Wait 10 sec to connect to serial console if USB connected
        absolute_time_t timerUSBInitEndTime = make_timeout_time_ms(LEADER_DETECT_TIMEOUT_MS);
        while (!time_reached(timerUSBInitEndTime)) {
            IS_USB_CONNECTED = stdio_usb_connected();
            if (IS_USB_CONNECTED) break;
        }
    #endif

    IS_LEADER = IS_USB_CONNECTED;
    IS_FOLLOWER = !IS_USB_CONNECTED;

    printf ("Program starting...\n");

    matrixInit();
    
    printf ("Initializing RGB...\n");
    rgbMatrix.initializeRGB(LED_PIN, RGBLED_NUM, false, RGBLIGHT_LIMIT_VAL);
    sleep_ms(100);
    
    printf ("Initializing Serial State\n");
    if (!IS_LEADER) serialLeaderInit();
    if (!IS_FOLLOWER) serialFollowerInit();
    sleep_ms(100);

    printf ("Starting main loop...\n");

    bool drawLEDsNextFrame = false;

    absolute_time_t timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);
    absolute_time_t timerAlertTimeout = make_timeout_time_ms(1000); // Alert ignored before 1sec

    while (true) {
        matrixScan();
        if (!IS_LEADER) {
            #ifndef DEBUG_MODE
                hid_task();
                tud_task();
            #endif
            
            // uint8_t data[2] = {100};
            // serialReadBytes(serialSM, data, 2);
            // if (data[0] != 0) {
            //     printf("Recieved: ");
            //     printString(data, 2);
            //     printf("\n");
            // }
        }
        if (!IS_FOLLOWER) {
        }

        // Generate Animation
        if (time_reached(timerAnimateRGB)) {
            if (!time_reached(timerAlertTimeout)) isAlertState = false; // Alert ignored before 1sec
            if (isAlertState) {
                RGB c = {255,0,0};
                rgbMatrix.setAll(c);
            } else {
                animateBreathe(&rgbMatrix);
            }
            
            timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);
            drawLEDsNextFrame = true;

        // Draw animation
        } else if (drawLEDsNextFrame) {
            rgbMatrix.draw();
            drawLEDsNextFrame = false;
        } else {
            countTest();  // Can't be same loop as LED stuff for some reason?  Sounds reasonble tho
        }
    }
    return 0;
}
