#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "lights.h"
#include "nrf_delay.h"
#include "virtual_timer.h"

int left_timer_id = 0;
int right_timer_id = 0;

bool _left = 0;
bool _right = 0;

void press_button(int pin, int presses) {
	int i;
	for (i = 0; i < presses; i++) {
		nrf_gpio_pin_set(TAIL_PIN);
		nrf_delay_ms(50);
		nrf_gpio_pin_clear(TAIL_PIN);
		nrf_delay_ms(50);
		nrf_gpio_pin_set(TAIL_PIN);
	}
}

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
	nrf_gpio_pin_clear(BRAKE_PIN);
	nrf_gpio_pin_clear(TAIL_PIN);

	//press_button(TAIL_PIN, 3);
}

void toggle_left(void) {
	nrf_gpio_pin_toggle(LEFT_PIN);
}

void toggle_right(void) {
	nrf_gpio_pin_toggle(RIGHT_PIN);
}

void set_left(bool set_val) {
	if (set_val != _left) {
		_left = set_val;
		toggle_flash_left();
	}
}

void set_right(bool set_val) {
	if (set_val != _right) {
		_right = set_val;
		toggle_flash_right();
	}
}

void set_brake(bool set_val) {
	nrf_gpio_pin_clear(BRAKE_PIN);
	if (set_val) {
		nrf_gpio_pin_toggle(BRAKE_PIN);
	}
}

void set_tail(bool set_val) {
	// if (set_val) {
	// 	press_button(TAIL_PIN, 1);
	// } else{
	// 	press_button(TAIL_PIN, 3);
	// }
	if (set_val) {
		nrf_gpio_pin_set(TAIL_PIN);
	} else {
		nrf_gpio_pin_clear(TAIL_PIN);
	}
}

void toggle_flash_left(void) {
	if (!left_timer_id && _left) {
		left_timer_id = virtual_timer_start_repeated(FLASH_INTERVAL, toggle_left);
	} else {
		virtual_timer_cancel(left_timer_id);
		nrf_gpio_pin_clear(LEFT_PIN);
		left_timer_id = 0;
	}
}

void toggle_flash_right(void) {
	if (!right_timer_id && _right) {
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