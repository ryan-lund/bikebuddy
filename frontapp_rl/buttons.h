#pragma once

#ifndef BUTTONS_VARS
#define BUTTONS_VARS
#define LEFT_TURN_BUTTON_PIN 28
#define RIGHT_TURN_BUTTON_PIN 29
#define START_RIDE_PIN_PIN 30
#endif

void buttons_init(void);

// Note: True = on, False = off
bool get_left_turn_button_state(void);

bool get_right_turn_button_state(void);

bool get_start_ride_button_state(void);