
// AVR Pin maps from arduino pins
#define D3 0
#define D2 1
#define D1 2
#define D0 3
#define D4 4
#define C6 5
#define D7 6
#define E6 7
#define B4 8
#define B5 9
#define B6 10
#define B3 14
#define B1 15
#define B2 16
// A0, A1, A2, A3
#define F7 18
#define F6 19
#define F5 20
#define F4 21

#define KEYS_ROWS 10
#define KEYS_COLS 6
#define MATRIX_ROWS 5
#define MATRIX_COLS 6
#define MATRIX_ROW_PINS { C6, D7, E6, B4, B5 }
#define MATRIX_COL_PINS { F6, F7, B1, B3, B2, B6 }
// #define MATRIX_ROW_PINS { 5, 6, 7, 8, 9 }
// #define MATRIX_COL_PINS { 19, 18, 15, 14, 16, 10 }


// #define MATRIX_ROWS 6
// #define MATRIX_COLS 2
// #define MATRIX_ROW_PINS { C6, D7, E6, B4, B5 }
// #define MATRIX_COL_PINS { B2, B6 }

#define DIODE_DIRECTION COL2ROW


#define LED_PIN 1
#define LED_LENGTH 38

#define RGB_MATRIX_ENABLE

#define RGB_DI_PIN 0
#define RGBLED_NUM 72
#define RGBLED_SPLIT { 36, 36 }
#define DRIVER_LED_TOTAL RGBLED_NUM


#define RGBLIGHT_LIMIT_VAL 120
#define RGBLIGHT_HUE_STEP 10
#define RGBLIGHT_SAT_STEP 17
#define RGBLIGHT_VAL_STEP 17


#define SCAN_INTERVAL 30
#define DEBOUNCE 5

//////// rev1.h
// /* USB Device descriptor parameter */
// #define VENDOR_ID       0xFC32
// #define PRODUCT_ID      0x0287
// #define DEVICE_VER      0x0001
// #define MANUFACTURER    JosefAdamcik
// #define PRODUCT         Sofle

// /* key matrix size */
// // Rows are doubled-up
// #define MATRIX_ROWS 10
// #define MATRIX_COLS 6

// // wiring of each half
// #define MATRIX_ROW_PINS { C6, D7, E6, B4, B5 }
// #define MATRIX_COL_PINS { F6, F7, B1, B3, B2, B6 }
// #define DIODE_DIRECTION COL2ROW

// #define TAPPING_TERM 100
// #define DEBOUNCE 5

// /* encoder support */
// #define ENCODERS_PAD_A { F5 }
// #define ENCODERS_PAD_B { F4 }
// #define ENCODERS_PAD_A_RIGHT { F4 }
// #define ENCODERS_PAD_B_RIGHT { F5 }
// #define ENCODER_RESOLUTION 2

// #define TAP_CODE_DELAY 10

// /* communication between sides */
// #define USE_SERIAL
// #define SERIAL_USE_MULTI_TRANSACTION
// #define SOFT_SERIAL_PIN D2

// #define NO_ACTION_MACRO
// #define NO_ACTION_FUNCTION