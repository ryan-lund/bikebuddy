#pragma once

/* Defines for blind spot monitor system */
#define BSM_VARS

#define BSM_LEFT_TRIG_PIN 4
#define BSM_LEFT_ECHO_PIN 5
#define BSM_RIGHT_TRIG_PIN 4
#define BSM_RIGHT_ECHO_PIN 5

#define BSM_THRESHOLD 122 // distance in cm
#define BSM_TIMEOUT	8 // timeout time ms
#define US_TO_CM 0.0343

/* Defines for lights */
#define LIGHTS_VARS

#define LEFT_PIN 30
#define RIGHT_PIN 31
#define TAIL_PIN 3
#define BRAKE_PIN 2

#define FLASH_INTERVAL 500000