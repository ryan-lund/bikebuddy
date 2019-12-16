#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_app_front.h"
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


static void nav_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    if (data[0] == 1)
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 0);
        NRF_LOG_INFO("Navigation Enabled");
    }
    else 
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 1);
        NRF_LOG_HEXDUMP_INFO(data, 32);
    }
}

static void blind_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    if (data[0] == 1)
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 6);
        NRF_LOG_INFO("Got Blind Spot");
    }
    else 
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 7);
        NRF_LOG_HEXDUMP_INFO(data, 32);
    }
}


float distance;
char direction[15];
static void direction_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    // distance = 11.32;
    // NRF_LOG_ERROR("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    NRF_LOG_HEXDUMP_INFO(&distance, sizeof(distance));
    memcpy(&distance, data, sizeof(distance));
    NRF_LOG_ERROR("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    NRF_LOG_HEXDUMP_INFO(&distance, sizeof(distance));
    memcpy(direction, data, sizeof(direction));
    NRF_LOG_INFO("%s", direction);
    // TODO
    if (data[0] == 1)
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 2);
        NRF_LOG_INFO("Navigation Enabled");
    }
    else 
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 3);
        NRF_LOG_HEXDUMP_INFO(data, 32);
    }
}

static void light_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    // distance = 11.32;
    // NRF_LOG_ERROR("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    // NRF_LOG_HEXDUMP_INFO(&distance, sizeof(distance));
    // memcpy(&distance, data, sizeof(distance));
    // NRF_LOG_ERROR("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    // NRF_LOG_HEXDUMP_INFO(&distance, sizeof(distance));
    // memcpy(direction, data, sizeof(direction));
    // NRF_LOG_INFO("%s", direction);
    // TODO
    if (data[0] == 1)
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 4);
        NRF_LOG_INFO("Navigation Enabled");
    }
    else 
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 5);
        NRF_LOG_HEXDUMP_INFO(data, 32);
    }
}

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
    init.nav_write_handler = nav_write_handler;
    init.direction_write_handler = direction_write_handler;
    init.blind_write_handler = blind_write_handler;
    init.light_write_handler = light_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}



/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    nrf_gpio_cfg_output(BRAKE_LED);
    nrf_gpio_cfg_output(LEFT_LED);
    nrf_gpio_cfg_output(RIGHT_LED);
    log_init();
    leds_init();
    timers_init();
    // buttons_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    nrf_delay_ms(3000);

    // Start execution.
    NRF_LOG_INFO("Front App Started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}


/**
 * @}
 */