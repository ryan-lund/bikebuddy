#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "bsm.h"
#include "backapp.h"
#include "virtual_timer.h"

uint32_t tick_distance(uint32_t stop, uint32_t start) {
  if (stop > start) {
    return stop - start;
  } else {
    return (INT_MAX - start) + stop;
  }
}

// need to supply timer defined in main by APP_TIMER_DEF(bsm_timer_id);
void bsm_init(void) {

    // Configure GPIOs
	nrf_gpio_pin_dir_set(BSM_LEFT_TRIG_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_dir_set(BSM_RIGHT_TRIG_PIN, NRF_GPIO_PIN_DIR_OUTPUT); 
	nrf_gpio_pin_dir_set(BSM_LEFT_ECHO_PIN, NRF_GPIO_PIN_DIR_INPUT);
	nrf_gpio_pin_dir_set(BSM_RIGHT_ECHO_PIN, NRF_GPIO_PIN_DIR_INPUT);

	// insure that all pins are set to high (due to n-transistor)
	nrf_gpio_pin_set(BSM_LEFT_TRIG_PIN);
	nrf_gpio_pin_set(BSM_RIGHT_TRIG_PIN); 
}

uint32_t bsm_get_single_dist(uint32_t trig, uint32_t echo) {
  uint32_t start = 0;
  uint32_t stop = 0;
  uint32_t timeout = read_timer();
  // send pulse to sensor
  NRF_LOG_INFO("Testing trig = %d, echo = %d", trig, echo);
  NRF_LOG_FLUSH();
  nrf_gpio_pin_set(trig);
  nrf_delay_us(20);
  nrf_gpio_pin_clear(trig);
  nrf_delay_us(12);
  nrf_gpio_pin_set(trig);
  while(!nrf_gpio_pin_read(echo)) {
    // if(read_timer() - timeout > 100000)
    //   return 777;
  }
  start = read_timer();
  while(nrf_gpio_pin_read(echo)) {
    // if(read_timer() - timeout > 100000)
    //   return 777;
  }
  stop = read_timer();

  NRF_LOG_INFO("Stop: %d, Start: %d", stop, start);
  NRF_LOG_FLUSH();
  float dist = 0.5 * US_TO_CM * tick_distance(stop, start);
  return (int)(dist + 0.5); // return rounded distance in cm
}

bsm_dist_t bsm_get_dist(void) {
  bsm_dist_t ret;

	ret.left_dist = bsm_get_single_dist(BSM_LEFT_TRIG_PIN, BSM_LEFT_ECHO_PIN);
  ret.right_dist = bsm_get_single_dist(BSM_LEFT_TRIG_PIN, BSM_LEFT_ECHO_PIN);

  return ret;
}

bsm_danger_t bsm_get_danger(void) {
  bsm_danger_t ret;

  ret.left_danger = bsm_get_single_dist(BSM_LEFT_TRIG_PIN, BSM_LEFT_ECHO_PIN) < BSM_THRESHOLD;
  ret.right_danger = bsm_get_single_dist(BSM_LEFT_TRIG_PIN, BSM_LEFT_ECHO_PIN) < BSM_THRESHOLD;

  return ret;
}