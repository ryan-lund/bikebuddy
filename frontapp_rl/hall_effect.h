#pragma once

#ifndef HALL_EFFECT_VARS
#define HALL_EFFECT_VARS
#define HALL_EFFECT_PIN 9
#define HALL_EFFECT_WHEEL_CIRCUMFERENCE 2105 // wheel circumference in mm
#define HALL_EFFECT_NUM_MAGNETS 1 // number of magnets on the wheel
#endif

void hall_effect_init(void);

void hall_effect_reset(void);

float hall_effect_get_dist(void);

float hall_effect_get_speed(void);