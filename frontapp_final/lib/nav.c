#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "nav.h"

uint32_t dist_timer_id = 0;
uint32_t turn_dir = 0; // 0 for straight, 1 for left, 2 for right)
// Meter unit
float distance_remaining = 0.0f;
bool nav_system_active = false;

float nav_set_distance(float distance) {
    distance_remaining = distance;
}

void update_distance_remaining(void) {
    distance_remaining -= // READ SPEED
    if (distance_remaining > 0.0) {
        nav_update_display_distance_remaining();
    } else {
        distance_remaining = 0.0;
    }
}

void nav_start(void) {
    dist_timer_id = virtual_timer_start_repeated(UPDATE_INTERVAL, update_distance_remaining);
    nav_system_active = true;
}

void nav_stop(void) {
    virtual_timer_cancel(dist_timer_id);
    nav_system_active = false;
}

// TODO: finalize 
void nav_update_waypoint() {
    // TODO: Break up data packet, update display for direction and road and distance
    nav_set_distance();
}

bool get_nav_system_active(void) {
    return nav_system_active;
}

// TODO: finalize
void nav_update_display_distance_remaining(void) {
    // UPDATE DISTANCE REMAINING ON DISPLAY (miles)
}

void get_distance_remaining(void) {
    return distance_remaining;
}

uint32_t get_turn_direction(void) {
    return turn_dir;
}
