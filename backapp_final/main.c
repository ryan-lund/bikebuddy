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
static void back_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * led_state, uint16_t len)
{
    NRF_LOG_HEXDUMP_INFO(led_state,32);
    NRF_LOG_INFO("%x", led_state[0]);
    if (led_state[0] == 1)
    {
        toggle_brakelight();
        NRF_LOG_INFO("Recieved toggle to brakelight!");
    } else if (led_state[0] == 2) {
        toggle_flash_left();
        NRF_LOG_INFO("Recieved toggle to left indicator!");
    } else if (led_state[0] == 3) {
        toggle_flash_right();
        NRF_LOG_INFO("Recieved toggle to right indicator!");
    } else if (led_state[0] == 4) {
        toggle_taillight();
        NRF_LOG_INFO("Recieved toggle to taillight");
    } else {
        NRF_LOG_INFO("Recieved toggle for nothing");
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
    init.back_write_handler = back_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    NRF_LOG_INFO("1 Back App Started.");
    leds_init();
    timers_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    NRF_LOG_INFO("2 Back App Started.");
    conn_params_init();
    bsm_init();
    virtual_timer_init();
    lights_init();
    nrf_delay_ms(3000);

    // Start execution.
    NRF_LOG_INFO("3 Back App Started.");
    advertising_start();

    bsm_danger_t bsm_danger;
    while (1) {
        bsm_danger = bsm_get_danger();
        NRF_LOG_INFO("Main Loop Started.");
        switch (left_bsm_state) {
            case LIGHT_OFF: {
                if (bsm_danger.left_danger) {
                    left_bsm_state = LIGHT_ON;
                    // TODO: WRITE A 0 ON BACK->FRONT BLE
                    ble_lbs_on_state_change(m_conn_handle, &m_lbs, 0);
                } else {
                    left_bsm_state = LIGHT_OFF;
                }
                break;
            }
            case LIGHT_ON: {
                if (!bsm_danger.left_danger) {
                    left_bsm_state = LIGHT_OFF;
                    // TODO: WRITE A 0 ON BACK->FRONT BLE
                    ble_lbs_on_state_change(m_conn_handle, &m_lbs, 0);
                } else {
                    left_bsm_state = LIGHT_ON;
                }
                break;
            }
        }

        switch (right_bsm_state) {
            case LIGHT_OFF: {
                if (bsm_danger.right_danger) {  
                    right_bsm_state = LIGHT_ON;
                    // TODO: WRITE A 1 ON BACK->FRONT BLE
                    ble_lbs_on_state_change(m_conn_handle, &m_lbs, 1);
                } else {
                    right_bsm_state = LIGHT_OFF;
                }
                break;
            }
            case LIGHT_ON: {
                if (!bsm_danger.right_danger) {
                    right_bsm_state = LIGHT_OFF;
                    // TODO: WRITE A 1 ON BACK->FRONT BLE
                    ble_lbs_on_state_change(m_conn_handle, &m_lbs, 1);
                } else {
                    right_bsm_state = LIGHT_ON;
                }
                break;
            }
        }

        nrf_delay_ms(250);
        NRF_LOG_FLUSH();
        idle_state_handle();
    }
}


/**
 * @}
 */