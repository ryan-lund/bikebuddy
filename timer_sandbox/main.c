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
#include "nrf_drv_twi.h"
#include "imu.h"
#include "front_peripherals.h"

#define SSD1306_CONFIG_VDD_PIN 04
#define SSD1306_CONFIG_SCL_PIN_0 05
#define SSD1306_CONFIG_SDA_PIN_0 28
#define SSD1306_CONFIG_SCL_PIN_1 29
#define SSD1306_CONFIG_SDA_PIN_1 31

/* Off board light states */
light_state_t left_bsm_state = LIGHT_OFF;
light_state_t right_bsm_state = LIGHT_OFF;
nrf_drv_twi_t m_twi_master0 = NRF_DRV_TWI_INSTANCE(0);
nrf_drv_twi_t m_twi_master1 = NRF_DRV_TWI_INSTANCE(1);
/**
 * @brief TWI initialization.
 */
void twi_init0 (void)
{
        ret_code_t err_code;

        const nrf_drv_twi_config_t twi_sensors_config = {
                .scl                = SSD1306_CONFIG_SCL_PIN_0,
                .sda                = SSD1306_CONFIG_SDA_PIN_0,
                .frequency          = NRF_TWI_FREQ_400K,
                .interrupt_priority = APP_IRQ_PRIORITY_LOWEST
        };

        //err_code = nrf_drv_twi_init(&m_twi_lis2dh12, &twi_lis2dh12_config, twi_handler, NULL);
        err_code = nrf_drv_twi_init(&m_twi_master0, &twi_sensors_config, NULL, NULL);    // twi in blocking mode.
        APP_ERROR_CHECK(err_code);

        nrf_drv_twi_enable(&m_twi_master0);
}

void twi_init1 (void)
{
        ret_code_t err_code;

        const nrf_drv_twi_config_t twi_sensors_config = {
                .scl                = SSD1306_CONFIG_SCL_PIN_1,
                .sda                = SSD1306_CONFIG_SDA_PIN_1,
                .frequency          = NRF_TWI_FREQ_400K,
                .interrupt_priority = APP_IRQ_PRIORITY_LOWEST
        };

        //err_code = nrf_drv_twi_init(&m_twi_lis2dh12, &twi_lis2dh12_config, twi_handler, NULL);
        err_code = nrf_drv_twi_init(&m_twi_master1, &twi_sensors_config, NULL, NULL);    // twi in blocking mode.
        APP_ERROR_CHECK(err_code);

        nrf_drv_twi_enable(&m_twi_master1);
}

/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_lbs     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
static void led_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * led_state)
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
    init.led_write_handler = led_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry.
 */
char dist_towp[] = "0.1 mi";
int main(void)
{
    // Initialize.
    log_init();
    leds_init();
    timers_init();
    buttons_init();
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
    nrf_delay_ms(3000);

    // Start execution.
    NRF_LOG_INFO("Back App Started.");
    advertising_start();
    twi_init0();
    twi_init1();

    unsigned char accel_calib_status = 0;
	u8 chip_id = 0;

    // Start execution.

    imu_init(&m_twi_master0);
    NRF_LOG_INFO("IMU INITIALIZED");
    double d_linear_accel_datax;

    bno055_read_chip_id(&chip_id);
	NRF_LOG_INFO("chip id: %x", chip_id);

    init_peripherals(&m_twi_master0, &m_twi_master1);

    // bsm_danger_t  bsm_danger;
    // bsm_dist_t bsm_dist;
    //toggle_flash_left()
    // Enter main loop.
    display_set_leftTurn(true);
    nrf_delay_ms(3000);
    display_set_direction(FRONT);
    NRF_LOG_INFO("%s", dist_towp);
    display_set_disttowp(dist_towp);
    nrf_delay_ms(1000);
    display_set_street("Stanford");
    display_set_speed("1 mph");
    // display_set_rightTurn(true);
    // nrf_delay_ms(3000);
    // display_set_leftBS(true);
    // nrf_delay_ms(3000);
    // display_set_rightBS(true);
    while (1) {
        //bsm_dist = bsm_get_dist();
        //bsm_danger = bsm_get_danger();

        //NRF_LOG_INFO("Left BSM Value: %d", bsm_dist.left_dist);
        //NRF_LOG_INFO("Right BSM Value: %d", bsm_dist.right_dist);

        /*switch (left_bsm_state) {
            case LIGHT_OFF: {
                if (bsm_danger.left_danger) {
                    left_bsm_state = LIGHT_ON;
                    // TODO: WRITE A 0 ON BACK->FRONT BLE
                } else {
                    left_bsm_state = LIGHT_OFF;
                }
                break;
            }
            case LIGHT_ON: {
                if (!bsm_danger.left_danger) {
                    left_bsm_state = LIGHT_OFF;
                    // TODO: WRITE A 0 ON BACK->FRONT BLE
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
                } else {
                    right_bsm_state = LIGHT_OFF;
                }
                break;
            }
            case LIGHT_ON: {
                if (!bsm_danger.right_danger) {
                    right_bsm_state = LIGHT_OFF;
                    // TODO: WRITE A 1 ON BACK->FRONT BLE
                } else {
                    right_bsm_state = LIGHT_ON;
                }
                break;
            }
        }*/


     
        idle_state_handle();
    }
}


/**
 * @}
 */