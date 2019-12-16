#include <stdbool.h>
#include <limits.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "boards.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "hall_effect.h"
#include "virtual_timer.h"

const float m_per_tick = ((float) HALL_EFFECT_WHEEL_CIRCUMFERENCE) / (HALL_EFFECT_NUM_MAGNETS * 1000);

volatile float dist = 0.0f;
volatile float speed = 0.0f;
volatile uint32_t last_start = 0;
volatile uint32_t time_elapsed = 0;

static uint32_t tick_distance(uint32_t stop, uint32_t start) {
  if (stop > start) {
    return stop - start;
  } else {
    return (INT_MAX - start) + stop;
  }
}

void hall_effect_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_INFO("PIN CHANGE DETECTED");
	uint32_t stop = read_timer();
    uint32_t time_delta = tick_distance(stop, last_start);
    time_elapsed += time_delta;
    dist += m_per_tick;
    speed = (m_per_tick / 1000.0f) / (((float) time_delta) / 3600000000.0f);
    last_start = read_timer();

    NRF_LOG_INFO("DIST IS" NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(dist));
    NRF_LOG_INFO("THERE ARE " NRF_LOG_FLOAT_MARKER " M PER TICK\r\n", NRF_LOG_FLOAT(m_per_tick));
}
/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
void hall_effect_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(HALL_EFFECT_PIN, &in_config, hall_effect_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(HALL_EFFECT_PIN, true);
    last_start = read_timer();
}

void hall_effect_reset(void) {
	dist = 0.0f;
	speed = 0.0f;
    time_elapsed = 0;
	last_start = read_timer();
}

// returns net distance traveled in m
float hall_effect_get_dist(void) {
	return dist;
}

// returns last seen speed in km/hr
float hall_effect_get_speed(void) {
	return speed;
}

// Returns total time elapsed in microseconds
uint32_t hall_effect_get_time(void) {
    return time_elapsed;
}
