/**
 * Code for Bike Buddy Back Module
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "types.h"
#include "lights.h"

static simple_ble_config_t ble_config = {
    // c0:98:e5:49:xx:xx
    .platform_id       = 0x49,    // used as 4th octect in device BLE address
    .device_id         = 0xBBBB,
    .adv_name          = "Bike Buddy Rear", // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

turn_light_state_t turn_light_state = TURN_OFF;
light_state_t tail_light_state = LIGHT_OFF;
light_state_t brake_light_state = LIGHT_OFF;

// 3db02924-b2a6-4d47-be1f-0f90ad62a048
// TODO: change this to be UUID of bike buddy service
static simple_ble_service_t bike_buddy_service = {{
    .uuid128 = {0x48,0xA0,0x62,0xAD,0x90,0x0F,0x1F,0xBE,
                0x47,0x4D,0xA6,0xB2,0x24,0x29,0xB0,0x3D}
}};

static simple_ble_char_t brake_char = {.uuid16 = 0x2925};
static simple_ble_char_t turn_char = {.uuid16 = 0x2926};
static simple_ble_char_t tail_light_char = {.uuid16 = 0x2927}

static uint8_t brake_char_data;
static uint8_t turn_char_data;
static uint8_t tail_light_char_data;

bool left = false;
bool right = false;
bool turn_off = false;
bool tail_on = false;
bool tail_off = false;
bool brake_on = false;
bool brake_off = false;

// Main application state
simple_ble_app_t* simple_ble_app;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
  if (simple_ble_is_char_event(p_ble_evt, &brake_char)) {
    if (brake_char_data == 0) {
      brake_off = true;
    } else if (brake_char_data == 1) {
      brake_on = true;
    }
  }

  if (simple_ble_is_char_event(p_ble_evt, &turn_char)) {
    if (turn_char_data == 0) {
      turn_off = true;
    } else if (turn_char_data == 1) {
      left = true;
    } else if (turn_char_data == 2) {
      right = true;
    }
  }

  if (simple_ble_is_char_event(p_ble_evt, &tail_light_char) {
    if (tail_light_char_data == 0) {
      tail_off = true;
    else if (tail_light_char_data == 1) {
      tail_on = true;
    }
  }  
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // BLE initialization
    simple_ble_app = simple_ble_init(&ble_config);
    simple_ble_add_service(&bike_buddy_service);

    // Add characteristics
    simple_ble_add_characteristic(1, 0, 0, 0,
        sizeof(brake_char_data), (uint8_t*) brake_char_data,
        &bike_buddy_service, &brake_char);
    simple_ble_add_characteristic(1, 0, 1, 0,
        sizeof(turn_char_data), (uint8_t*) turn_char_data,
        &bike_buddy_service, &turn_char);
    simple_ble_add_characteristic(1, 0, 1, 0,
        sizeof(tail_light_char_data), (uint8_t*) tail_light_char,
        &bike_buddy_service, &tail_light_char);

    // Lights initilization 
    init_lights_back();


    while(1) {
      /**
      FSM for the rear turn light
      **/
      switch (turn_light_state) {
          case TURN_OFF: {
            if (left) {
              left = false;
              tail_light_state = TURN_LEFT;
              toggle_flash_left();
            } else if (right) {
              right = false;
              tail_light_state = TURN_RIGHT;
              toggle_flash_right();
            } else {
              tail_light_state = TURN_OFF;
            }
          }
          case TURN_LEFT: {
            if (turn_off) {
              turn_off = false;
              tail_light_state = TURN_OFF;
              toggle_flash_left();
            } else if (right) {
              right = false;
              tail_light_state = TURN_RIGHT;
              toggle_flash_left();
              toggle_flash_right();
            } else {
              tail_light_state = TURN_LEFT;
            }
          }
          case TURN_RIGHT: {
            if (turn_off) {
              turn_off = false;
              tail_light_state = TURN_OFF;
              toggle_flash_left();
            } else if (left) {
              left = false;
              tail_light_state = TURN_LEFT;
              toggle_flash_right();
              toggle_flash_left();
            } else {
              tail_light_state = TURN_RIGHT;
            }
          }
      }


      /**
      FSM for the tail light
      **/
      switch (tail_light_state) {
          case LIGHT_OFF: {
              if (tail_on) {
                  tail_on = false;
                  tail_light_state = LIGHT_ON;
                  toggle_taillight();
              } else {
                  tail_light_state = LIGHT_OFF;
              }
          }
          case LIGHT_ON: {
              if (tail_off) {
                  tail_off = false;
                  tail_light_state = LIGHT_OFF;
                  toggle_taillight();
              } else {
                  tail_light_state = LIGHT_ON;
              }
          }
      }

      switch (brake_light_state) {
        case LIGHT_OFF: {
          if (brake_on) {
              brake_on = false;
              brake_light_state = LIGHT_ON;
              toggle_brakelight();
          } else {
              brake_light_state = LIGHT_OFF;
          }
        }
        case LIGHT_ON: {
          if (brake_off) {
            brake_off = false;
            brake_light_state = LIGHT_OFF;
            toggle_brakelight();
          } else {
            brake_light_state = LIGHT_ON;
          }
        }
      }
    }
}
