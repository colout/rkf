#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t col;
    uint8_t row;
} keypos_t;

typedef struct {
    keypos_t key;
    bool     pressed;
    uint16_t time;
} keyevent_t;

typedef struct {
    keyevent_t event;
    uint16_t keycode;
} keyrecord_t;


void process_record(keyrecord_t *record);