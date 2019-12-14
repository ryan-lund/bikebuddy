#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "lights.h"
#include "virtual_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

int left_timer_id = 0;
int right_timer_id = 0;

void lights_init(void) {
	// configure pins as outputs
	// 4: left
	// 5: right
	// 29: taillight
	// 30: brake
	nrf_gpio_pin_dir_set(LEFT_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(RIGHT_PIN, NRF_GPIO_PIN_DIR_OUTPUT); 
	nrf_gpio_pin_dir_set(TAIL_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(BRAKE_PIN, NRF_GPIO_PIN_DIR_OUTPUT);

	// insure that all pins are set to low
	nrf_gpio_pin_clear(LEFT_PIN);
	nrf_gpio_pin_clear(RIGHT_PIN); 
	nrf_gpio_pin_clear(TAIL_PIN);
	nrf_gpio_pin_clear(BRAKE_PIN);
}

void toggle_left(void) {
	// NRF_LOG_INFO("Executing timer");
	nrf_gpio_pin_toggle(LEFT_PIN);
}

void toggle_right(void) {
	nrf_gpio_pin_toggle(RIGHT_PIN);
}

void toggle_flash_left(void) {
	NRF_LOG_INFO("STARTING TIMER");
	if (!left_timer_id) {
		left_timer_id = virtual_timer_start_repeated(FLASH_INTERVAL, toggle_left);
	} else {
		virtual_timer_cancel(left_timer_id);
		nrf_gpio_pin_clear(LEFT_PIN);
		left_timer_id = 0;
	}
}

void toggle_flash_right(void) {
	if (!right_timer_id) {
		right_timer_id = virtual_timer_start_repeated(FLASH_INTERVAL, toggle_right);
	} else {
		virtual_timer_cancel(right_timer_id);
		nrf_gpio_pin_clear(RIGHT_PIN);
		right_timer_id = 0;
	}
}

void toggle_taillight(void) {
	nrf_gpio_pin_toggle(TAIL_PIN);
}

void toggle_brakelight(void) {
	nrf_gpio_pin_toggle(BRAKE_PIN);
}