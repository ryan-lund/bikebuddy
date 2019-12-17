#pragma once

/* Defines for blind spot monitor system */
#define BSM_VARS

#define BSM_LEFT_TRIG_PIN 3
#define BSM_LEFT_ECHO_PIN 5
#define BSM_RIGHT_TRIG_PIN 19
#define BSM_RIGHT_ECHO_PIN 29

#define BSM_THRESHOLD 122 // distance in cm
#define BSM_TIMEOUT	8 // timeout time ms
#define US_TO_CM 0.0343

/* Defines for lights */
#define LIGHTS_VARS

#define LEFT_PIN 30
#define RIGHT_PIN 31
#define TAIL_PIN 22//23
#define BRAKE_PIN 3

/* Defines for Hall Effect Driver */
#define HALL_EFFECT_VARS

#define HALL_EFFECT_PIN 20
#define HALL_EFFECT_WHEEL_CIRCUMFERENCE 2105 // wheel circumference in mm
#define HALL_EFFECT_NUM_MAGNETS 1 // number of magnets on the wheel

#define FLASH_INTERVAL 500000
