
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();

    hid_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch(report_id)
  {
    case REPORT_ID_KEYBOARD:
    {
      // use to avoid send multiple consecutive zero report for keyboard
      static bool has_keyboard_key = false;

      if ( btn )
      {
        uint8_t keycode[6] = { 0 };
        keycode[0] = HID_KEY_A;

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
        has_keyboard_key = true;
      }else
      {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
      }
    }
    break;

    case REPORT_ID_MOUSE:
    {
      int8_t const delta = 5;

      // no button, right + down, no scroll, no pan
      tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
    }
    break;

    case REPORT_ID_CONSUMER_CONTROL:
    {
      // use to avoid send multiple consecutive zero report
      static bool has_consumer_key = false;

      if ( btn )
      {
        // volume down
        uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
        has_consumer_key = true;
      }else
      {
        // send empty key report (release key) if previously has key pressed
        uint16_t empty_key = 0;
        if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
        has_consumer_key = false;
      }
    }
    break;

    case REPORT_ID_GAMEPAD:
    {
      // use to avoid send multiple consecutive zero report for keyboard
      static bool has_gamepad_key = false;

      hid_gamepad_report_t report =
      {
        .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
        .hat = 0, .buttons = 0
      };

      if ( btn )
      {
        report.hat = GAMEPAD_HAT_UP;
        report.buttons = GAMEPAD_BUTTON_A;
        tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

        has_gamepad_key = true;
      }else
      {
        report.hat = GAMEPAD_HAT_CENTERED;
        report.buttons = 0;
        if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
        has_gamepad_key = false;
      }
    }
    break;

    default: break;
  }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  // Remote wakeup
  if ( tud_suspended() && btn )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }else
  {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD, btn);
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id, board_button_read());
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

/*
#include <stdio.h>
#include "config.h"

#include "pico/stdlib.h"
#include "rgb_matrix.h"
#include "helpers.h"
#include "boards/sparkfun_promicro.h"

#include "keyboard.h"
#include "usb_hid.h"
#include "matrix_scan.h"

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"


#define TIME_BETWEEN_ANIMATE_RGB 16

RgbMatrix rgbMatrix;

// // Breathe
uint8_t brightness = 0;
int8_t breatheDirection = 1;
void animateBreathe() {
    brightness = brightness + breatheDirection;
    if (brightness >= 255) breatheDirection = -1;
    if (brightness <= 0) breatheDirection = 1;

    RGB c = {brightness,0,0};
    rgbMatrix.setAll(c);
}
// End breathe

// Might be required for hid reports?

// Invoked when received GET_REPORT control request.
//
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request.
uint16_t hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO: implement me.
    //printf("hid_get_report_cb: id=%d type=%d buf=%p len=%d\n", report_id, report_type, buffer, reqlen);
    return 0;
}

// Invoked when received SET_REPORT control request or received data on OUT
// endpoint ( Report ID = 0, Type = 0 )
void hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    // TODO: set LED based on CAPLOCK, NUMLOCK etc...
    //printf("hid_set_report_cb: id=%d type=%d buf=%p size=%d\n", report_id, report_type, buffer, bufsize);
}
//


int main() {
    // Init for USB
    board_init();
    tusb_init();
    stdio_init_all();

    printf ("Program starting...\n");
    sleep_ms(100);
    matrixInit();
    
    printf ("Initializing RGB...\n");
    rgbMatrix.initializeRGB(LED_PIN, LED_LENGTH, false, RGBLIGHT_LIMIT_VAL);
    sleep_ms(100);
    
    printf ("Starting main loop...\n");

    bool drawLEDsNextFrame = false;

    absolute_time_t timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);

    while (true) {
        // So we can time out other things
        absolute_time_t timerFullLoop = get_absolute_time();
        
        // Takes under 200us no input.  300us with input
        //uint32_t t_start = time_us_32(); // Start timer wrap
        matrixScan();
        hid_task();
        //uint32_t t_end = time_us_32();  printf("%d\n", t_end - t_start);  // end timer wrap

        // Do non-matrix scan stuff.  Only do one per loop.
        if (time_reached(timerAnimateRGB)) {
            animateBreathe();
            timerAnimateRGB = make_timeout_time_ms(TIME_BETWEEN_ANIMATE_RGB);
            drawLEDsNextFrame = true;
        } else if (drawLEDsNextFrame) {
            // Split the animation calculation and draw code between frames to get more matrix scans
            rgbMatrix.draw();
            drawLEDsNextFrame = false;
        }

        ////Log loop time
        //serialLog(absolute_time_diff_us(timerFullLoop, get_absolute_time()));
        //printf (" main loop time\n");
    }
    sleep_ms(16);
    return 0;
}
*/