#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_twi_mngr.h"
#include "nrf_gpio.h"
#include "display.h"
#include "lights.h"

void* init_lights(void*) {
	// onfigure pins as outputs
	// 4: left
	// 5: right
	// 28: headlight
	// 29: taillight
	// 30: brake
	nrf_gpio_cfg_output(LEFT_PIN);
	nrf_gpio_cfg_output(RIGHT_PIN); 
	nrf_gpio_cfg_output(HEAD_PIN);
	nrf_gpio_cfg_output(TAIL_PIN);
	nrf_gpio_cfg_output(BRAKE_PIN);

	// insure that all pins are set to low
	nrf_gpio_pin_clear(LEFT_PIN);
	nrf_gpio_pin_clear(RIGHT_PIN); 
	nrf_gpio_pin_clear(HEAD_PIN);
	nrf_gpio_pin_clear(TAIL_PIN);
	nrf_gpio_pin_clear(BRAKE_PIN);
}

void* toggle_left(void*) {
	return
}

void* toggle_right(void*) {
	return
}

void* toggle_headlight(void*) {
	nrf_gpio_pin_toggle(HEAD_PIN);
}

void* toggle_taillight(void*) {
	nrf_gpio_pin_toggle(TAIL_PIN);
}

void* toggle_brake(void*) {
	nrf_gpio_pin_toggle(BRAKE_PIN);
}