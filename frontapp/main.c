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

#define LUX_THRESHOLD 1000

static simple_ble_config_t ble_config = {
    // c0:98:e5:49:xx:xx
    .platform_id       = 0x49,    // used as 4th octect in device BLE address
    .device_id         = 0xBBBF,  // BBBF where F is for Front      
    .adv_name          = "Bike Buddy Front", // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

// Application variables
turn_light_state_t turn_light_state = TURN_OFF;
light_state_t head_light_state = LIGHT_OFF;
uint32_t light_value;

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

// Main application state
simple_ble_app_t* simple_ble_app;

void ble_evt_write(ble_evt_t const* p_ble_evt) {

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
    simple_ble_add_characteristic(0, 1, 0, 0,
        sizeof(brake_char_data), (uint8_t*) brake_char_data,
        &bike_buddy_service, &brake_char);
    simple_ble_add_characteristic(0, 1, 0, 0,
        sizeof(turn_char_data), (uint8_t*) turn_char_data,
        &bike_buddy_service, &turn_char);
    simple_ble_add_characteristic(0, 1, 0, 0,
        sizeof(tail_light_char_data), (uint8_t*) tail_light_char,
        &bike_buddy_service, &tail_light_char);

    // Lights initilization 
    init_lights_front();

    while(1) {
        light_value = read_light_sensor(); //TODO: add proper function call here;
        switch(head_light_state) {
            case LIGHT_OFF: {
                if (light_value > LUX_THRESHOLD) {
                    toggle_headlight();
                    turn_char_data = 0;
                    simple_ble_notify_char(&tail_light_char);
                    head_light_state = LIGHT_ON;
                } else {
                    head_light_state = LIGHT_OFF;
                }
            }
            case LIGHT_ON: {
                if (light_value < LUX_THRESHOLD) {
                    toggle_headlight();
                    turn_char_data = 1;
                    simple_ble_notify_char(&tail_light_char);
                    head_light_state = LIGHT_OFF;
                } else {
                    head_light_state = LIGHT_OFF;
                }
            }
        }

    }
}
