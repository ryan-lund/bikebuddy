#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "lights.h"
#include "virtual_timer.h"

int left_timer_id = 0;
int right_timer_id = 0;


void lights_init(void) {
	// configure pins as outputs
	// 28: Left
	// 29: Right
	// 30: Head
	nrf_gpio_pin_dir_set(LEFT_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(RIGHT_PIN, NRF_GPIO_PIN_DIR_OUTPUT); 
	nrf_gpio_pin_dir_set(HEAD_PIN, NRF_GPIO_PIN_DIR_OUTPUT);

	// insure that all pins are set to low
	nrf_gpio_pin_clear(LEFT_PIN);
	nrf_gpio_pin_clear(RIGHT_PIN); 
	nrf_gpio_pin_clear(HEAD_PIN);
}

void press_button(int pin, int presses) {
	int i;
	for (i = 0; i < presses; i++) {
		nrf_gpio_pin_set(HEAD_PIN);
		nrf_delay_ms(50);
		nrf_gpio_pin_clear(HEAD_PIN);
		nrf_delay_ms(50);
		nrf_gpio_pin_set(HEAD_PIN);
	}
}

void toggle_left(void) {
	nrf_gpio_pin_toggle(LEFT_PIN);
}

void set_left(void) {
	nrf_gpio_pin_set(LEFT_PIN);
}

void clear_left(void) {
	nrf_gpio_pin_clear(LEFT_PIN);
}

void toggle_right(void) {
	nrf_gpio_pin_toggle(RIGHT_PIN);
}

void set_right(void) {
	nrf_gpio_pin_set(RIGHT_PIN);
}

void clear_right(void) {
	nrf_gpio_pin_clear(RIGHT_PIN);
}

void set_headlight(bool set_val) {
	if (set_val) {
		nrf_gpio_pin_clear(HEAD_PIN);
		nrf_delay_ms(1);
		nrf_gpio_pin_set(HEAD_PIN);
		nrf_delay_ms(1);
		nrf_gpio_pin_clear(HEAD_PIN);
	} else {
		nrf_gpio_pin_clear(HEAD_PIN);
		nrf_delay_ms(1);
		nrf_gpio_pin_set(HEAD_PIN);
		nrf_delay_ms(1);
		nrf_gpio_pin_clear(HEAD_PIN);
	}
}

void toggle_flash_left(void) {
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

void toggle_headlight(void) {
	nrf_gpio_pin_toggle(HEAD_PIN);
}