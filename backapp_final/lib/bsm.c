#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "app_timer.h"

#include "bsm.h"
#include "backapp.h"

m_timer_id_t const *app_timer_id;

uint32_t app_timer_us(uint32_t ticks)
{
  // eg. (7 + 1) * 1000 / 32768
  //   = 8000 / 32768
  //   = 0.24414062
  float numerator = ((float)APP_TIMER_PRESCALER + 1.0f) * 1.0f;
  float denominator = (float)APP_TIMER_CLOCK_FREQ;
  float us_per_tick = numerator / denominator;

  uint32_t us = us_per_tick * ticks;

  return us;
}

// need to supply timer defined in main by APP_TIMER_DEF(bsm_timer_id);
void* bsm_init(app_timer_id_t const *timer_id) {
	m_timer_id = timer_id;

	ret_code_t err_code;

  // Create timers
  err_code = app_timer_create(m_timer_id,
                              APP_TIMER_MODE_SINGLE_SHOT,
                              null);
  APP_ERROR_CHECK(err_code);

    // Configure GPIOs
	nrf_gpio_pin_dir_set(BSM_LEFT_TRIG_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(BSM_RIGHT_TRIG_PIN, NRF_GPIO_PIN_DIR_OUTPUT); 
	nrf_gpio_pin_dir_set(BSM_LEFT_ECHO_PIN, NRF_GPIO_PIN_DIR_INPUT);
	nrf_gpio_pin_dir_set(BSM_RIGHT_ECHO_PIN, NRF_GPIO_PIN_DIR_INPUT);

	// insure that all pins are set to low
	nrf_gpio_pin_clear(BSM_LEFT_TRIG_PIN);
	nrf_gpio_pin_clear(BSM_RIGHT_TRIG_PIN); 
}

uint32_t bsm_get_single_dist(uint32_t trig, uint32_t echo) {
  uint32_t start = 0;
  uint32_t stop = 0;
  // send pulse to sensor
  nrf_gpio_pin_clear(trig);
  nrf_delay_us(20);
  nrf_gpio_pin_set(trig);
  nrf_delay_us(12);
  nrf_gpio_pin_clear(trig);
  nrf_delay_us(20); 

  // time high response
  while(!nrf_gpio_pin_read(echo));

  // Start timer
  app_timer_start(m_timer_id,
                  APP_TIMER_TICKS(BSM_TIMEOUT),
                  null);

  start = app_timer_cnt_get();

  while(nrf_gpio_pin_read(echo));

  stop = app_timer_cnt_get();
  app_timer_stop(m_timer_id);

  float dist = 0.5 * US_TO_CM * app_timer_us(app_timer_cnt_diff_compute(stop, start));

  return (int)(dist + 0.5); // return rounded distance in cm
}

bsm_dist_t bsm_get_dist(void) {
  bsm_dist_t ret;

	ret.left_dist = bsm_get_single_dist(BSM_LEFT_TRIG_PIN, BSM_LEFT_ECHO_PIN);
  ret.right_dist = bsm_get_single_dist(BSM_RIGHT_TRIG_PIN, BSM_RIGHT_ECHO_PIN);

  return ret;
}

bsm_danger_t bsm_get_danger(void) {
  bsm_danger_t ret;

  ret.left_danger = bsm_get_single_dist(BSM_LEFT_TRIG_PIN, BSM_LEFT_ECHO_PIN) < BSM_THRESHOLD;
  ret.right_dist = bsm_get_single_dist(BSM_RIGHT_TRIG_PIN, BSM_RIGHT_ECHO_PIN) < BSM_THRESHOLD;

  return ret;
}