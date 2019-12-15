#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "buttons.h"

void buttons_init(void) {
	nrf_gpio_pin_dir_set(LEFT_TURN_BUTTON_PIN, NRF_GPIO_PIN_DIR_INPUT);
	nrf_gpio_pin_dir_set(RIGHT_TURN_BUTTON_PIN, NRF_GPIO_PIN_DIR_INPUT); 
	nrf_gpio_pin_dir_set(START_RIDE_PIN_PIN, NRF_GPIO_PIN_DIR_INPUT);

	nrf_gpio_pin_clear(LEFT_TURN_BUTTON_PIN);
	nrf_gpio_pin_clear(RIGHT_TURN_BUTTON_PIN); 
	nrf_gpio_pin_clear(START_RIDE_PIN_PIN);
}

// Note: True = on, False = off
bool get_left_turn_button_state(void) {
	return nrf_gpio_pin_read(LEFT_TURN_BUTTON_PIN);
}

bool get_right_turn_button_state(void) {
	return nrf_gpio_pin_read(RIGHT_TURN_BUTTON_PIN);
}

bool get_start_ride_button_state(void) {
	return nrf_gpio_pin_read(START_RIDE_PIN_PIN);
}