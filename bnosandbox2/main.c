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
#include "imu.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_twi.h"

#include "SSD1306.h"
#include "Adafruit_GFX.h"
#include "buffers.h"

// nrf_drv_twi_t* _m_twi_master;
#define SSD1306_CONFIG_VDD_PIN 04
#define SSD1306_CONFIG_SCL_PIN_0 05
#define SSD1306_CONFIG_SDA_PIN_0 28
#define SSD1306_CONFIG_SCL_PIN_1 29
#define SSD1306_CONFIG_SDA_PIN_1 30


static uint8_t buffer0[SSD1306_128_32_LCDWIDTH * SSD1306_128_32_LCDHEIGHT / 8];


static void led_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * led_state)
{
    if (led_state[0] == 1)
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 1);
        NRF_LOG_INFO("Received LED ON! BRAKE");
    }
    else if (led_state[0]== 2)
    {
        NRF_LOG_INFO("Received LED OFF! LEFT");
    }
    else if (led_state[0]== 3)
    {
        NRF_LOG_INFO("Received LED OFF! RIGHT");
    }
    else 
    {
        ble_lbs_on_state_change(m_conn_handle, &m_lbs, 5);
        NRF_LOG_HEXDUMP_INFO(led_state, 32);
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
    init.led_write_handler = led_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}

// static void button_event_handler(uint8_t pin_no, uint8_t button_action)
// {
//     ret_code_t err_code;

//     switch (pin_no)
//     {
//         case LEDBUTTON_BUTTON:
//             NRF_LOG_INFO("Send button state change.");
//             err_code = ble_lbs_on_button_change(m_conn_handle, &m_lbs, button_action);
//             if (err_code != NRF_SUCCESS &&
//                 err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
//                 err_code != NRF_ERROR_INVALID_STATE &&
//                 err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
//             {
//                 APP_ERROR_CHECK(err_code);
//             }
//             break;

//         default:
//             APP_ERROR_HANDLER(pin_no);
//             break;
//     }
// }


// static void buttons_init(void)
// {
//     ret_code_t err_code;

//     //The array must be static because a pointer to it will be saved in the button handler module.
//     static app_button_cfg_t buttons[] =
//     {
//         {LEDBUTTON_BUTTON, false, BUTTON_PULL, button_event_handler}
//     };

//     err_code = app_button_init(buttons, ARRAY_SIZE(buttons),
//                                BUTTON_DETECTION_DELAY);
//     APP_ERROR_CHECK(err_code);
// }



const nrf_drv_twi_t m_twi_master0 = NRF_DRV_TWI_INSTANCE(0);
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

void testdrawcircle(nrf_drv_twi_t const m_twi_master, uint8_t i2caddr, uint8_t *buffer) {
    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_setRotation(0);
    for (int16_t i=0; i< Adafruit_GFX_height()/2; i+=2)
    {
            Adafruit_GFX_drawCircle(Adafruit_GFX_width()/2, Adafruit_GFX_height()/2, i, WHITE);
            SSD1306_display(m_twi_master, i2caddr, buffer, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
            nrf_delay_ms(200);
    }
}

void testdrawLeftTriangle(uint8_t *buffer)
{
//     void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
// void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_drawRect(20, 0, 40, 15, WHITE);
    Adafruit_GFX_fillRect(20, 0, 40, 15, WHITE);
    Adafruit_GFX_drawTriangle(0, 15, 40, 32, 80, 15, WHITE);
    Adafruit_GFX_fillTriangle(0, 15, 40, 32, 80, 15, WHITE);

}

void testdrawBSTriangle(uint8_t *buffer)
{
//     void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
// void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_drawTriangle(96, 16, 128, 32, 128, 0, WHITE);
    Adafruit_GFX_fillTriangle(96, 16, 128, 32, 128, 0, WHITE);

}
/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    nrf_gpio_cfg_output(BRAKE_LED);
    nrf_gpio_cfg_output(LEFT_LED);
    nrf_gpio_cfg_output(RIGHT_LED);
    nrf_gpio_cfg_output(8);
    nrf_gpio_cfg_output(7);
    twi_init0();
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
    NRF_LOG_INFO("BNO SANDBOX2 started.");
    advertising_start();

    unsigned char accel_calib_status = 0;
	u8 chip_id = 0;

    // Start execution.

    imu_init(&m_twi_master0);
    NRF_LOG_INFO("IMU INITIALIZED");
    double d_linear_accel_datax;

    bno055_read_chip_id(&chip_id);
	NRF_LOG_INFO("chip id: %x", chip_id);

    SSD1306_begin(m_twi_master0, SSD1306_I2C_ADDRESS, SSD1306_SWITCHCAPVCC, false, SSD1306_128_32, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    NRF_LOG_INFO("INITIALIZED SSD");
    SSD1306_clearDisplay(buffer0, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer0);
    NRF_LOG_INFO("INITIALIZED adafruit");

    SSD1306_clearDisplay(buffer0, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    NRF_LOG_INFO("cleared display");
    Adafruit_GFX_drawBitmap(0, 0,  el_logo, 128, 32, 1);
    SSD1306_display(m_twi_master0, SSD1306_I2C_ADDRESS, buffer0, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    nrf_delay_ms(1000);

    SSD1306_clearDisplay(buffer0, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    NRF_LOG_INFO("cleared display");
    testdrawLeftTriangle(buffer0);
    testdrawBSTriangle(buffer0);
    //testdrawcircle(m_twi_master0, SSD1306_I2C_ADDRESS, buffer0);
    //testdrawText(buffer0);
    SSD1306_display(m_twi_master0, SSD1306_I2C_ADDRESS, buffer0, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    //SSD1306_display(m_twi_master0, SSD1306_I2C_ADDRESS, buffer0, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);


    NRF_LOG_INFO("DISPLAY0_1 INITIALIZED");


    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
        // imu_reinit();
        // NRF_LOG_INFO("IMU INITIALIZED");
        bno055_convert_double_linear_accel_x_msq(&d_linear_accel_datax);
        //NRF_LOG_INFO("lin accel %d", d_linear_accel_datax);
        NRF_LOG_FLUSH();
    }
}


/**
 * @}
 */