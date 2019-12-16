#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "virtual_timer.h"
#include "frontapp.h"
#include "trip.h"
#include "front_peripherals.h"
#include "nav.h"

uint32_t nav_dist_timer_id = 0;
uint32_t turn_dir = 0; // 0 for straight, 1 for left, 2 for right, 3 for END)
// Meter unit
float distance_remaining = 0.0f;
float last_distance = 0.0f;
bool nav_system_active = false;

void nav_start(void) {
    nav_system_active = true;
}

void nav_stop(void) {
    nav_system_active = false;
}

// TODO: finalize 
void nav_update_waypoint(uint8_t* data) {
    uint32_t distance;
    uint8_t direction;
    char road[15];
    memcpy(&distance, data, sizeof(distance));
    memcpy(&direction, data+5, sizeof(direction));
    memcpy(road, data+6, 15 * sizeof(char));
    distance_remaining = distance;
    // TODO: UPDATE DISPLAY FOR DIR AND ROAD
}

bool get_nav_system_active(void) {
    return nav_system_active;
}

void nav_update_display_distance_remaining(void) {
    float curr_distance = get_dist_traveled();
    distance_remaining -= (curr_distance - last_distance);
    if (distance_remaining < 0.0f) {
        distance_remaining = 0.0f;
    }
    last_distance = curr_distance;
    char disttowp_buff[4];
    snprintf(disttowp_buff, sizeof(disttowp_buff), "%.2f", distance_remaining);
    display_set_disttowp(disttowp_buff);
}

uint32_t get_distance_remaining(void) {
    return distance_remaining;
}

uint32_t get_turn_direction(void) {
    return turn_dir;
}
