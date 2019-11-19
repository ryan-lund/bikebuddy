/**
 * Code for Bike Buddy Back Module
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "types.h"

static simple_ble_config_t ble_config = {
    // c0:98:e5:49:xx:xx
    .platform_id       = 0x49,    // used as 4th octect in device BLE address
    .device_id         = 0xBBBB,
    .adv_name          = "Bike Buddy Rear", // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

// 3db02924-b2a6-4d47-be1f-0f90ad62a048
static simple_ble_service_t map_service = {{
    .uuid128 = {0x48,0xA0,0x62,0xAD,0x90,0x0F,0x1F,0xBE,
                0x47,0x4D,0xA6,0xB2,0x24,0x29,0xB0,0x3D}
}};

static simple_ble_char_t brake_char = {.uuid16 = 0x2925};
static simple_ble_char_t turn_char = {.uuid16 = 0x2926};


turn_light_state_t turn_light_state = OFF;
light_state_t tail_light_state = OFF;
light_state_t brake_light_state = OFF;

bool left = false;
bool right = false;
bool off = false;
bool tail = false;
bool brake = false;

// Main application state
simple_ble_app_t* simple_ble_app;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
  if (simple_ble_is_char_event(p_ble_evt, &brake_char)) {
    if (brake_light_state == 1) {
      brake = true;
    } else {
      brake = false;
    }
  }

  if (simple_ble_is_char_event(p_ble_evt, &rear_turn_light_char)) {
    if (rear_turn_light_state == 0) {
      off = true;
    } else if (rear_turn_light_state == 1) {
      left = true;
    } else if (rear_turn_light_state == 2) {
      right = true;
    }
  }  
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    switch (turn_light_state) {
        case OFF: {
          if (left) {
            left = false;
            tail_light_state = LEFT;
            toggle_flash_left();
          } else if (right) {
            right = false;
            tail_light_state = RIGHT;
            toggle_flash_right();
          } else {
            off = false;
            tail_light_state = OFF;
          }
        }
        case LEFT: {
          if (off) {
            off = false;
            tail_light_state = left;
            toggle_flash_left();
          } else if (right) {
            right = false;
            tail_light_state = RIGHT;
            toggle_flash_left();
            toggle_flash_right();
          } else {
            left = false;
            tail_light_state = LEFT;
          }
        }
        case RIGHT: {
          if (off) {
            off = false;
            tail_light_state = left;
            toggle_flash_left();
          } else if (left) {
            left = false;
            tail_light_state = LEFT;
            toggle_flash_right();
            toggle_flash_left();
          } else {
            right = false;
            tail_light_state = RIGHT;
          }
        }
    }

    switch (tail_light_state) {
        case OFF: {
            if (tail) {
                tail_light_state = ON;
                toggle_tail();
            }
        }
        case ON: {
            if (!tail) {
                tail_light_state = OFF;
                toggle_tail();
            }
        }
    }

    switch (brake_light_state) {
        case OFF: {
            if (brake) {
                brake_light_state = ON;
                toggle_brake();
            }
        }
        case ON: {
            if (!brake) {
                brake_light_state = OFF;
                toggle_brake();
            }
        }
    }
}
