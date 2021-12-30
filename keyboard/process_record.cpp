//#include "usb_hid.h"

//#include "bsp/board.h"
//#include "tusb.h"

#include "process_record.h"
void process_record(keyrecord_t *record) {
    printf("\n\nProcess Record:\nRow:%d\nCol:%d\nPressed:%b\nTime:%d\nCode:%d\n", 
        record->event.key.row, 
        record->event.key.col, 
        record->event.pressed,
        record->event.time,
        record->keycode);
}