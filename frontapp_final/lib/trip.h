#pragma once
#include "frontapp.h"

void update_time(void);

void update_dist(void);

void start_time_rec(void);

void start_dist_rec(void);

void start_elev_rec(void);

void stop_time_rec(void);

void stop_dist_rec(void);

void stop_elev_rec(void);

uint32_t get_time_elapsed(void);

float get_dist_traveled(void);

float get_elev_change(void);