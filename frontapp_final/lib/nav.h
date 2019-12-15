#pragma once

void update_distance_remaining(void);

void nav_start(void);

void nav_stop(void);

void nav_update_waypoint(uint8_t* data);

bool get_nav_system_active(void);

void nav_update_display_distance_remaining(void);

void get_distance_remaining(void);

uint32_t get_turn_direction(void);