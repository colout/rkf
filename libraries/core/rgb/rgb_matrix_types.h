#include "config.h"
#include "color.h"

#pragma once

// Key reactive
#define LED_HITS_TO_REMEMBER 8

typedef struct {
    uint8_t  count;
    uint8_t  x[LED_HITS_TO_REMEMBER];
    uint8_t  y[LED_HITS_TO_REMEMBER];
    uint8_t  index[LED_HITS_TO_REMEMBER];
    uint16_t tick[LED_HITS_TO_REMEMBER];
} last_hit_t;

typedef enum rgb_task_states { STARTING, RENDERING, FLUSHING, SYNCING } rgb_task_states;

typedef uint8_t led_flags_t;

typedef struct {
    uint8_t     iter;
    led_flags_t flags;
    bool        init;
} effect_params_t;

#define NO_LED 255

#define LED_FLAG_ALL 0xFF
#define LED_FLAG_NONE 0x00
#define LED_FLAG_MODIFIER 0x01
#define LED_FLAG_UNDERGLOW 0x02
#define LED_FLAG_KEYLIGHT 0x04
#define LED_FLAG_INDICATOR 0x08

typedef struct {
    uint8_t x;
    uint8_t y;
} led_point_t;

typedef struct {
    uint8_t     matrix_co[KEYS_ROWS][KEYS_COLS];
    led_point_t point[DRIVER_LED_TOTAL];
    uint8_t     flags[DRIVER_LED_TOTAL];
} led_config_t;

typedef union {
    uint32_t raw;
    struct {
        uint8_t     enable : 2;
        uint8_t     mode : 6;
        HSV         hsv;
        uint8_t     speed;  // EECONFIG needs to be increased to support this
        led_flags_t flags;
    };
} rgb_config_t;
