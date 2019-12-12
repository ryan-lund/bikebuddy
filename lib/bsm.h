#pragma once
#ifndef BSM_H_
#define BSM_H_

#ifndef BSM_VARS
#define BSM_VARS
#define BSM_LEFT_TRIG_PIN 4
#define BSM_LEFT_ECHO_PIN 5
#define BSM_LEFT_TRIG_PIN 6
#define BSM_LEFT_ECHO_PIN 7

#define BSM_THRESHOLD 122 // distance in cm
#define BSM_TIMEOUT	8 // timeout time ms
#define US_TO_CM 0.0343
#endif

typedef struct {
	uint32_t left_dist,
	uint32_t right_dist;
} bsm_dist_t;

typedef struct {
	bool left_danger,
	bool right_danger
} bsm_danger_t;

void* bsm_init(void);

bsm_dist_t bsm_get_dist(void);

bsm_danger_t bsm_get_danger(void);

#endif // BSM_H_