#include "process_record.h"
#include "config.h"
#include "keycodes.h"
#include "serial_1wire.h"

#ifndef DEBUG_MODE
    #include "hid.h"
#endif

void process_record(keyrecord_t *record) {
    // Key code is KC_UNDEFINED if freshly matrix scanned.  
    //   If not defined yet, we do the lookup 
    if (record->keycode == KC_UNDEFINED) record->keycode = keymaps[0][record->event.key.row][record->event.key.col];

    #ifndef DEBUG_MODE
        hidkb_report_code(record->keycode, record->event.pressed);
    #else
        printf(
            "\n\nProcess Record:\nRow:%d\nCol:%d\nPressed:%b\nTime:%d\nCode:%d\n",
            record->event.key.col, 
            record->event.key.row, 
            record->event.pressed, 
            record->event.time, 
            record->keycode
        );
    #endif

}