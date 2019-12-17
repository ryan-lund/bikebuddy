#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_app_back.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_lbs.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "types.h"
#include "hall_effect.h"
#include "bsm.h"
#include "lights.h"
#include "virtual_timer.h"

/* Off board light states */
light_state_t left_bsm_state = LIGHT_OFF;
light_state_t right_bsm_state = LIGHT_OFF;

/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_lbs     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
static void backlight_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    if (data[0] == 0) {
        set_brake(true);
        NRF_LOG_INFO("Turned on brake");
    } else if (data[0] == 1) {
        set_brake(false);
        NRF_LOG_INFO("Turned off brake");
    } else if (data[0] == 2) {
        set_left(true);
        NRF_LOG_INFO("Turned on left");
    } else if (data[0] == 3) {
        set_left(false);
        NRF_LOG_INFO("Turned off left");
    } else if (data[0] == 4) {
        set_right(true);
        NRF_LOG_INFO("Turned on right");
    } else if (data[0] == 5) {
        set_right(false);
        NRF_LOG_INFO("Turned off right");
    } else if (data[0] == 6) {
        set_tail(true);
        NRF_LOG_INFO("Turned on tail");
    } else if (data[0] == 7) {
        set_tail(false);
        NRF_LOG_INFO("Turned off tail");
    }


}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    ret_code_t         err_code;
    ble_lbs_init_t     init     = {0};
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize LBS.
    init.backlight_write_handler = backlight_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for applicatino main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    leds_init();
    timers_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    bsm_init();
    virtual_timer_init();
    lights_init();
    hall_effect_init();
    nrf_delay_ms(3000);

    // Start execution.
    NRF_LOG_INFO("Back App Started.");
    NRF_LOG_FLUSH();
    advertising_start();

    nrf_gpio_pin_dir_set(21, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(22, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_clear(21);
    nrf_gpio_pin_clear(21);

    bsm_danger_t bsm_danger;
    bsm_dist_t bsm_dist;
    uint8_t count = 0;
    while (1) {
        nrf_delay_ms(250);
    
        switch (left_bsm_state) {
            case LIGHT_OFF: {
                if (bsm_danger.left_danger) {
                    left_bsm_state = LIGHT_ON;
                    // TODO: WRITE A 0 ON BACK->FRONT BLE
                    send_blindspot_warning(m_conn_handle, &m_lbs, 0);
                    nrf_gpio_pin_set(22);
                } else {
                    left_bsm_state = LIGHT_OFF;
                    nrf_gpio_pin_clear(22);
                }
                break;
            }
            case LIGHT_ON: {
                if (!bsm_danger.left_danger) {
                    left_bsm_state = LIGHT_OFF;
                    // TODO: WRITE A 0 ON BACK->FRONT BLE
                    send_blindspot_warning(m_conn_handle, &m_lbs, 0);
                    nrf_gpio_pin_clear(22);
                } else {
                    left_bsm_state = LIGHT_ON;
                    nrf_gpio_pin_set(22);
                }
                break;
            }
        }

        switch (right_bsm_state) {
            case LIGHT_OFF: {
                if (bsm_danger.right_danger) {  
                    right_bsm_state = LIGHT_ON;
                    // TODO: WRITE A 1 ON BACK->FRONT BLE
                    send_blindspot_warning(m_conn_handle, &m_lbs, 1);
                    nrf_gpio_pin_set(21);
                } else {
                    right_bsm_state = LIGHT_OFF;
                    nrf_gpio_pin_clear(21);
                }
                break;
            }
            case LIGHT_ON: {
                if (!bsm_danger.right_danger) {
                    right_bsm_state = LIGHT_OFF;
                    // TODO: WRITE A 1 ON BACK->FRONT BLE
                    send_blindspot_warning(m_conn_handle, &m_lbs, 1);
                    nrf_gpio_pin_clear(21);
                } else {
                    right_bsm_state = LIGHT_ON;
                    nrf_gpio_pin_set(21);
                }
                break;
            }
        }

        if (count == 3) {
            float speed = hall_effect_get_speed();
            float distance = hall_effect_get_dist();
            uint32_t time = hall_effect_get_time();
            uint8_t data[8];
            NRF_LOG_INFO("Sending Speed Distance Notification");
            memcpy(data, &speed, sizeof(speed));
            memcpy(data+4, &distance, sizeof(distance));
            memcpy(data+8, &time, sizeof(time));
            ble_send_speed(m_conn_handle, &m_lbs, data);
            bsm_danger = bsm_get_danger();
            bsm_dist = bsm_get_dist();
            NRF_LOG_INFO("Left: %d", bsm_dist.left_dist);
            NRF_LOG_INFO("Right: %d",bsm_dist.right_dist);
            count = 0;
        } else {
            count += 1;
        }
        nrf_delay_ms(250);
        NRF_LOG_FLUSH();
    }
}


// /**
//  * @}
//  */