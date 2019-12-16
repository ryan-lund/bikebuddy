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
#include "nrfx_gpiote.h"
#include "nrfx_saadc.h"
#include "app_util_platform.h"


// Log Headers
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Our Own Lib Files
#include "buttons.h"
#include "nav.h"
#include "front_peripherals.h"
#include "trip.h"
#include "tsl2561.h"
#include "types.h"
#include "virtual_timer.h"
#include "lights.h"
#include "nrf_drv_twi.h"

// TODO: FIX ME
#define HEADLIGHT_THRESHOLD  100
#define TURNLIGHT_POLAR_TURN_THRESHOLD  1234 // Righ is pos, left is neg
#define BRAKELIGHT_DECEL_THRESHOLD  1234


#define GYROZ_THRESHOLD 300
#define ANGLE_THRESHOLD 15

#define BOARD_SPARKFUN_NRF52840_MINI    1

NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Indicates if operation on TWI has ended. */
volatile bool _twi_init = false;

/* TWI instance. */

#define SSD1306_CONFIG_VDD_PIN 04
#define SSD1306_CONFIG_SCL_PIN_0 05
#define SSD1306_CONFIG_SDA_PIN_0 28
#define SSD1306_CONFIG_SCL_PIN_1 29
#define SSD1306_CONFIG_SDA_PIN_1 31
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


char direction_nav[20];
static void nav_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    memcpy(direction_nav, data, sizeof(direction_nav));
    NRF_LOG_INFO("%s", direction_nav);
    display_set_street((char *)data);
}

static void blind_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    if (data[0] == 0) {
        display_set_leftBS(true);
    } else if (data[0] == 1) {
        display_set_leftBS(false);
    } else if (data[0] == 2) {
        display_set_rightBS(true);
    } else if (data[0] == 3) {
        display_set_rightBS(false);
    }
}

float distance;
char distance_string[8];
static void distance_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    NRF_LOG_INFO("distance");
    memcpy(&distance, data, sizeof(distance));
    NRF_LOG_HEXDUMP_INFO(data, 20);
    NRF_LOG_INFO("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    memcpy(&distance_string, data+4, sizeof(distance_string));
    NRF_LOG_INFO("%s", distance_string);
    display_set_disttowp(distance_string);
}

static void direction_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    // distance = 11.32;
    // NRF_LOG_ERROR("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    // NRF_LOG_HEXDUMP_INFO(&distance, sizeof(distance));
    // memcpy(&distance, data, sizeof(distance));
    // NRF_LOG_ERROR("Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(distance));
    // NRF_LOG_HEXDUMP_INFO(&distance, sizeof(distance));
    // memcpy(direction_nav, data, sizeof(direction_nav));
    // NRF_LOG_INFO("%s", direction_nav);
    // TODO
    if (data[0] == 0) {
        display_set_direction(FRONT);
    } else if (data[0] == 1) {
        display_set_direction(LEFT);
    } else if (data[0] == 2) {
        display_set_direction(RIGHT);
    } else if (data[0] == 3) {
        display_set_direction(ARRIVE);
    } else {
        display_set_direction(NONE);
    }
}

static void light_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
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
    init.distance_write_handler = distance_write_handler;
    init.blind_write_handler = blind_write_handler;
    init.light_write_handler = light_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}

static void twi_scan(void) {
  ret_code_t err_code;
  uint8_t address;
  uint8_t sample_data = 0;
  bool detected_device = false;

  //NRF_LOG_INFO("TWI scanner started.");
  
  if (!_twi_init) {
    NRF_LOG_INFO("TWI is not init");
    return;
  }

  for (address = 1; address <= 127; address++)
  {
    nrf_twi_mngr_transfer_t const test_transfer[] = {
        NRF_TWI_MNGR_READ(address, sample_data, 1, 0),
    };

    err_code = nrf_twi_mngr_perform(&twi_mngr_instance, NULL, test_transfer, 1, NULL);
      if (err_code == NRF_SUCCESS)
      {
          detected_device = true;
          NRF_LOG_INFO("TWI device detected at address 0x%x.", address);
          NRF_LOG_FLUSH();
      }
  }

  if (!detected_device)
  {
      NRF_LOG_INFO("No device was found.");
      NRF_LOG_FLUSH();
  }
}

// ADC SETUP
// // callback for SAADC events
// void saadc_callback (nrfx_saadc_evt_t const * p_event) {
//   // don't care about adc callbacks
// }

// // sample a particular analog channel in blocking mode
// nrf_saadc_value_t sample_value (uint8_t channel) {
//   nrf_saadc_value_t val;
//   ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
//   APP_ERROR_CHECK(error_code);
//   return val;
// }

// static void adc_init(void) {
//     ret_code_t error_code = NRF_SUCCESS;
//     // initialize analog to digital converter
//     nrfx_saadc_config_t saadc_config = NRFX_SAADC_DEFAULT_CONFIG;
//     saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
//     error_code = nrfx_saadc_init(&saadc_config, saadc_callback);
//     APP_ERROR_CHECK(error_code);

//     // initialize analog inputs
//     // configure with 0 as input pin for now
//     nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(0);
//     channel_config.gain = NRF_SAADC_GAIN1_6; // input gain of 1/6 Volts/Volt, multiply incoming signal by (1/6)
//     channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6 Volt reference, input after gain can be 0 to 0.6 Volts

//     // specify input pin and initialize that ADC channel
//     channel_config.pin_p = BUCKLER_ANALOG_ACCEL_X;
//     error_code = nrfx_saadc_channel_init(X_CHANNEL, &channel_config);
//     APP_ERROR_CHECK(error_code);
// }

// Configuring initial states
general_state_t trip_state = OFF;
general_state_t head_light_state = OFF;
general_state_t nav_state = OFF;
turn_light_state_t turn_light_state = TURN_OFF;

// booleans for buttons
bool switch_state_left = false;
bool switch_state_right = false;
bool ride_button_state = false;

// booleans for turning when navigation is active
bool nav_turn_right = false;
bool nav_turn_left = false;

// Variables for lux value
uint32_t tsl2561_lux;

volatile int16_t result = 0;
volatile float precise_result = 0;
/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    leds_init();
    lights_init();
    timers_init();
    // init_lights_front();
    // init_lights_back();
    virtual_timer_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    // twi_init(); // IMPORTANT
    buttons_init(); // IMPORTANT
    twi_init0();
    twi_init1();
    init_peripherals(&m_twi_master0, &m_twi_master1);
    nrf_delay_ms(3000);

    // Start execution.
    NRF_LOG_INFO("Front App Started.");
    advertising_start();
    NRF_LOG_FLUSH();
    // twi_scan();
    // tsl2561_init(&twi_mngr_instance);

    double* roll;
    double* gyro_z;
    // Enter main loop.
    while (true) {
        // NRF_LOG_INFO("Main Loop");
        // Display updates
        // TODO: Replace fn with real one
        // In fn make sure that it is in miles/hr
        // display_update_speed();

        // FSM for Trip Details
        // ride_button_state = get_start_ride_button_state();

        if (ble_connected) {
            nrf_delay_ms(50);

            ride_button_state = false;
            switch (trip_state) {
                case OFF: {
                    if (ride_button_state) {
                        // Initalize trip variables
                        start_time_rec();
                        start_dist_rec();
                        // start_elev_rec();
                        trip_state = ON;
                    }
                    break;
                }

                case ON: {
                    if (!ride_button_state) {
                        // Stop trip
                        stop_time_rec();
                        stop_dist_rec();
                        // stop_elev_rec();
                        // TODO: Call function to change displays (format unknown)
                        // TODO: uint32_t get_time_elapsed(void) 
                        // TODO: float get_dist_traveled(void)
                        // TODO: float get_elev_change(void)
                        trip_state = OFF;
                    }
                    break;
                }
            }

            roll = get_roll_degrees();
            gyro_z = get_gyro_z();
            if (*roll > ANGLE_THRESHOLD || *gyro_z > GYROZ_THRESHOLD) {
                display_set_rightTurn(true);
                //NRF_LOG_INFO("THRESH LEFT REACHED");
            } else {
                display_set_rightTurn(false);
            }

            if (*roll < -ANGLE_THRESHOLD || *gyro_z < -GYROZ_THRESHOLD) {
                display_set_leftTurn(true);
                //NRF_LOG_INFO("THRESH right REACHED");
            } else {
                display_set_leftTurn(false);
            }


            // // FSM for headlight
            // tsl2561_lux = tsl2561_get_lux();
            // NRF_LOG_INFO("%d",tsl2561_lux);
            // switch (head_light_state) {
            //     case OFF: {
            //         // If LUX is below threshold
            //         if (tsl2561_lux < HEADLIGHT_THRESHOLD) {
            //             // TURN LIGHT ON
            //             toggle_headlight();
            //             // TODO: toggle_taillight_char();
            //             head_light_state = ON;
            //         } else {
            //             head_light_state = OFF;
            //         }
            //         break;
            //     }

            //     case ON: {
            //         // If LUX is above threshold
            //         if (tsl2561_lux > HEADLIGHT_THRESHOLD) {
            //             // TURN LIGHT OFF
            //             toggle_headlight();
            //             // TODO: toggle_taillight_char();
            //             head_light_state = OFF;
            //         } else {
            //             head_light_state = ON;
            //         }
            //         break;
            //     }
            // }

            

            // // FSM for turnlights.
            // // TODO: Replace fn with real one
            // // polar_accel = bno055_get_polar_acceleration();
            // float polar_accel = 0;
            // // The physical switch for manual turn signals. Overrides automatic signals when active (left or right).
            // switch_state_left = nrf_gpio_pin_read(9);
            // switch_state_right = nrf_gpio_pin_read(10);
            // // switch_state_right = false;
            // // switch_state_left = false;
            // switch (turn_light_state) {
            //     case TURN_OFF: {
            //         if (!switch_state_right && (switch_state_left || nav_turn_left || polar_accel < -TURNLIGHT_POLAR_TURN_THRESHOLD)) {
            //             // If current polar_acceleration pass threshold (negative for left)
            //             toggle_flash_left();
            //             display_set_leftTurn(true);
            //             // TODO: sendtoggle_rearturn_left_char();
            //             turn_light_state = TURN_LEFT;
            //         } else if (switch_state_right || nav_turn_right || polar_accel > TURNLIGHT_POLAR_TURN_THRESHOLD) {
            //             // If current polar_acceleration pass threshold (negative for left)
            //             toggle_flash_right();
            //             display_set_rightTurn(true);
            //             // TODO: sendtoggle_rearturn_right_char();
            //             turn_light_state = TURN_RIGHT;
            //         } else {
            //             turn_light_state = TURN_OFF;
            //         }
            //         break;
            //     }

            //     case TURN_LEFT: {
            //         if (!switch_state_right && (switch_state_left || nav_turn_left || polar_accel < -TURNLIGHT_POLAR_TURN_THRESHOLD)) {
            //             // Do nothing
            //             turn_light_state = TURN_LEFT;
            //         } else if (switch_state_right || nav_turn_right || polar_accel > TURNLIGHT_POLAR_TURN_THRESHOLD) {
            //             // If current polar_acceleration pass threshold (negative for left)
            //             // First toggle left
            //             toggle_flash_left();
            //             display_set_leftTurn(false);
            //             // TODO: sendtoggle_rearturn_left_char();

            //             // Then toggle right
            //             toggle_flash_right();
            //             display_set_rightTurn(true);
            //             // TODO: sendtoggle_rearturn_right_char();
            //             turn_light_state = TURN_RIGHT;
            //         } else {
            //             // If no longer turning, toggle lights off
            //             toggle_flash_left();
            //             display_set_leftTurn(false);
            //             // TODO: sendtoggle_rearturn_left_char();
            //             turn_light_state = TURN_OFF;
            //         }
            //         break;
            //     }

            //     case TURN_RIGHT: {
            //         if (!switch_state_right && (switch_state_left || nav_turn_left || polar_accel < -TURNLIGHT_POLAR_TURN_THRESHOLD)) {
            //             // If current polar_acceleration pass threshold (negative for left)
            //             // First  toggle right
            //             toggle_flash_right();
            //             display_set_rightTurn(false);
            //             // TODO: sendtoggle_rearturn_right_char();
            //             // TODO: Toggle display right turn

            //             // Then toggle left
            //             toggle_flash_left();
            //             display_set_leftTurn(true);
            //             // TODO: sendtoggle_rearturn_left_char();
            //             turn_light_state = TURN_LEFT;
            //         } else if (switch_state_right || nav_turn_right || polar_accel > TURNLIGHT_POLAR_TURN_THRESHOLD) {
            //             // Do nothing
            //             turn_light_state = TURN_RIGHT;
            //         } else {
            //             // If no longer turning, toggle lights off
            //             toggle_flash_right();
            //             display_set_rightTurn(false);
            //             // TODO: sendtoggle_rearturn_right_char();
            //             turn_light_state = TURN_OFF;
            //         }
            //         break;
            //     }
            // }

            // FSM for nav system
            switch (nav_state) {
                case OFF: {
                    // If nav is active, move to ON state
                    if (get_nav_system_active()) {
                        nav_state = ON;
                    }
                    break;
                }

                case ON: {
                    if (!get_nav_system_active()) {
                        nav_state = OFF;
                    } else {
                        // Automatic turn signals when expecting imminent turn
                        float temp_dist = get_distance_remaining();
                        // TODO: Define these somewhere else
                        if (temp_dist < 10) {
                            uint32_t turn_dir = get_turn_direction();
                            if (turn_dir == 1) {
                                nav_turn_left = true;
                            } else if (turn_dir == 2) {
                                nav_turn_right = true;
                            } else {
                                nav_turn_left = false;
                                nav_turn_right = false;
                            }
                        } else {
                            nav_turn_left = false;
                            nav_turn_right = false;
                        }
                    }
                    break;
                }
            }
            NRF_LOG_FLUSH();
        } else {
            nrf_delay_ms(200);
            NRF_LOG_INFO("Not connected to BLE yet");
            NRF_LOG_FLUSH();
        }
    }
}


/**
 * @}
 */
