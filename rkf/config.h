#pragma once

#include <stdio.h>
#include "keycodes.h"

// Global variables
extern bool IS_USB_CONNECTED;  // Detect USB
extern bool IS_LEADER;         // aka "Master"
extern bool IS_FOLLOWER;       // aka "Slave"
extern uint serialSM;          // Serial state machine for 1wire

// Debug mode = Disable USB, HID to enable printf to serial out
#define DEBUG_MODE
#define IS_LEADER_OVERRIDE true

// AVR Pin maps from arduino pins (onboard LED Pins not included)
// Left Side
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

//Right Side
#define F4 29
#define F5 28
#define F6 27
#define F7 26
#define B1 22
#define B3 20
#define B2 23
#define B6 21

#define KEYS_ROWS 5
#define KEYS_COLS 12
#define MATRIX_ROWS 5
#define MATRIX_COLS 6
#define MATRIX_ROW_PINS { C6, D7, E6, B4, B5 }
#define MATRIX_COL_PINS { F6, F7, B1, B3, B2, B6 }

// #define DIODE_DIRECTION COL2ROW

#define LED_PIN 0

// #define RGB_MATRIX_ENABLE

#define RGB_DI_PIN 0
#define RGBLED_NUM 72
#define RGBLED_SPLIT { 36, 36 }
#define DRIVER_LED_TOTAL RGBLED_NUM
#define SPLIT_LEFT

#define RGBLIGHT_LIMIT_VAL 120
#define RGBLIGHT_HUE_STEP 10
#define RGBLIGHT_SAT_STEP 17
#define RGBLIGHT_VAL_STEP 17

// Microseconds between row scan
#define SCAN_INTERVAL_US 30
// Number of full matrix scans to wait after key press
#define DEBOUNCE_COUNT 5
// Total scan time = ~(SCAN_INTERVAL_US * ROW_COUNT)

// Serial
#define SOFT_SERIAL_PIN D2

enum layer_codes {
  LC_RAISE = SAFE_RANGE,
  LC_LOWER
};

/*
 * QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |  `   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | TAB  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LShift|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------| MUTE  |    |DISCORD|------+------+------+------+------+------|
 * | LCTR |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |LShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *            | Bspc | WIN  |LOWER | Enter| /Space  /       \Enter \  |SPACE |RAISE | RCTR | RAlt |
 *            |      |      |      |      |/       /         \      \ |      |      |      |      |
 *            `----------------------------------'           '------''---------------------------'
 */
const uint16_t keymaps[][KEYS_ROWS][KEYS_COLS] = {
    {

  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                        KC_6, KC_7,   KC_8,    KC_9,    KC_0,    KC_GRV,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,   KC_I,    KC_O,   KC_P,    KC_BSPC,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_LSFT,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,   KC_K,    KC_L,   KC_SCLN, KC_QUOT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_LCTL, KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_MUTE,  KC_MUTE,KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_LSFT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_BSPC, KC_LGUI, LC_RAISE, KC_SPC,  KC_ENT   ,     KC_SPC, KC_ENT, LC_LOWER, KC_RCTL, KC_RALT
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
    }
};