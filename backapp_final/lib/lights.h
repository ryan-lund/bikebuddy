#pragma once
#include "backapp.h"

#ifndef LIGHTS_VARS
#define LIGHTS_VARS
#define LEFT_PIN 30
#define RIGHT_PIN 28
#define TAIL_PIN 29
#define BRAKE_PIN 4
#define FLASH_INTERVAL 500000
#endif

void* init_lights(void);

void* toggle_left(void);

void* toggle_right(void);

void* toggle_flash_left(void);

void* toggle_flash_right(void);

void* toggle_taillight(void);

void* toggle_brakelight(void);
