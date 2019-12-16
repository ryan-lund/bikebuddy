#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "trip.h"
#include "nav.h"
#include "virtual_timer.h"

uint32_t time_elapsed = 0;
float distance_traveled = 0.0f;

void set_time_elapsed(uint32_t time) {
    time_elapsed = time;
}

void set_dist_traveled(float distance) {
    distance_traveled = distance;
    nav_update_display_distance_remaining();
} 

uint32_t get_time_elapsed(void) {
    return time_elapsed;
}

float get_dist_traveled(void) {
    return distance_traveled;
}

float get_avg_speed(void) {
    return ((float) distance_traveled / time_elapsed);
}