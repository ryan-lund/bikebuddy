#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "trip.h"
#include "virtual_timer.h"

uint32_t time_timer_id = 0;
uint32_t trip_dist_timer_id = 0;

uint32_t time_elapsed = 0;
float distance_traveled = 0.0f;

float start_elev = 0.0f;
float stop_elev = 0.0f;

void update_time (void) {
    time_elapsed += 1;
}

void calculate_dist(void) {
    float speed = 0; // READ SPEED
    // TODO 
    distance_traveled += speed;
}

void start_time_rec(void) {
    time_elapsed = 0;
    time_timer_id = virtual_timer_start_repeated(UPDATE_INTERVAL, update_time);
}

void start_dist_rec(void) {
    distance_traveled = 0.0f;
    trip_dist_timer_id = virtual_timer_start_repeated(UPDATE_INTERVAL, calculate_dist);
}

void start_elev_rec(void) {
    start_elev = 0;// READ FROM ALT SENSOR
}

void stop_time_rec(void) {
    virtual_timer_cancel(time_timer_id);
}

void stop_dist_rec(void) {
    virtual_timer_cancel(trip_dist_timer_id);
}

void stop_elev_rec(void) {
    stop_elev = 5;// READ FROM ALT SENSOR
}

uint32_t get_time_elapsed(void) {
    return time_elapsed;
}

float get_dist_traveled(void) {
    return distance_traveled;
}

float get_elev_change(void) {
    return stop_elev - start_elev;
}