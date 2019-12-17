#pragma once

void nav_start(void);

void nav_stop(void);

void set_nav_direction(uint32_t dir);

void set_nav_distance_remaining (float dist_remain);

bool get_nav_system_active(void);

void nav_update_display_distance_remaining(void);

uint32_t get_distance_remaining(void);

uint32_t get_turn_direction(void);