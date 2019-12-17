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
#include "front_peripherals.h"
#include "nav.h"
#include "trip.h"
#include "tsl2561.h"
#include "types.h"
#include "virtual_timer.h"
#include "lights.h"
#include "nrf_drv_twi.h"

// Defines for sensor thresholds
#define HEADLIGHT_THRESHOLD  2200

#define GYROZ_THRESHOLD 300
#define ANGLE_THRESHOLD 15

#define NAV_AUTOMATIC_TURN_DISTANCE 10

#define BOARD_SPARKFUN_NRF52840_MINI    1
#define PHOTORESISTOR_ADC_PIN NRF_SAADC_INPUT_AIN6

NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

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


char street_string[20];
static void street_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    memcpy(street_string, data, sizeof(street_string));
    NRF_LOG_INFO("%s", street_string);
    display_set_street((char *)data);
}

static void blind_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    bool left_blind = (data[0] >> 1) & 1;
    bool right_blind = data[0] & 1;
    if (left_blind) {
        display_set_leftBS(true);
    } else {
        display_set_leftBS(false);
    }
    if (right_blind) {
        display_set_rightBS(true);
    } else {
        display_set_rightBS(false);
    }
}

float distance;
char distance_string[8];
static void distance_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
    display_set_disttowp(distance_string);
}

static void direction_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t * data, uint16_t len)
{
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
    if (data[0] == 1) {
        ble_send_backlight_command(m_conn_handle, &m_lbs, 4);
        NRF_LOG_INFO("Navigation Enabled");
    } else {
        ble_send_backlight_command(m_conn_handle, &m_lbs, 5);
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
    init.street_write_handler = street_write_handler;
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
// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
}

// sample a particular analog channel in blocking mode

nrf_saadc_value_t sample_light_level (uint8_t channel) {
  nrf_saadc_value_t val;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
  APP_ERROR_CHECK(error_code);
  return val;
}

static void adc_init(void) {
    ret_code_t error_code = NRF_SUCCESS;
    // initialize analog to digital converter
    nrfx_saadc_config_t saadc_config = NRFX_SAADC_DEFAULT_CONFIG;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
    error_code = nrfx_saadc_init(&saadc_config, saadc_callback);
    APP_ERROR_CHECK(error_code);

    // initialize analog inputs
    // configure with 0 as input pin for now
    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(0);
    channel_config.gain = NRF_SAADC_GAIN1_6; // input gain of 1/6 Volts/Volt, multiply incoming signal by (1/6)
    channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6 Volt reference, input after gain can be 0 to 0.6 Volts

    // specify input pin and initialize that ADC channel
    channel_config.pin_p = PHOTORESISTOR_ADC_PIN;
    error_code = nrfx_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(error_code);
}

bool automatic_turn_left = false;
bool automatic_turn_right = false;
uint32_t left_turn_timer_id = 0;
uint32_t right_turn_timer_id = 0;

void cancel_left_turn_timer (void) {
    if (left_turn_timer_id) {
        virtual_timer_cancel(left_turn_timer_id);
        left_turn_timer_id = 0;
    }
    automatic_turn_left = false;
}

void cancel_right_turn_timer (void) {
    if (right_turn_timer_id) {
        virtual_timer_cancel(right_turn_timer_id);
        right_turn_timer_id = 0;
    }
    automatic_turn_right = false;
}

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    leds_init();
    lights_init();
    timers_init();
    adc_init();
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
    buttons_init(); // IMPORTANT
    twi_init0();
    twi_init1();
    init_peripherals(&m_twi_master0, &m_twi_master1);
    nrf_delay_ms(3000);

    // Start execution.
    NRF_LOG_INFO("Front App Started.");
    advertising_start();
    NRF_LOG_FLUSH();

    // Variables used within FSM

    // Configuring initial states
    general_state_t trip_state = OFF;
    general_state_t head_light_state = OFF;
    general_state_t brake_light_state = OFF;
    general_state_t nav_state = OFF;
    turn_light_state_t turn_light_state = TURN_OFF;

    // Variables for turn FSM
    double* roll;
    double* gyro_z;
    bool intended_left = false;
    bool intended_right =  false;
    nrf_saadc_value_t light_val = 0;

    // booleans for buttons
    bool switch_state_left = false;
    bool switch_state_right = false;
    bool ride_button_state = false;

    // Variables for brake light FSM
    double* acceleration;

    // booleans for turning when navigation is active
    bool nav_turn_right = false;
    bool nav_turn_left = false;

    // Enter main loop.
    while (true) {
        // FSM for Trip Details
        // ride_button_state = get_start_ride_button_state();

        if (ble_connected) {
            nrf_delay_ms(250);
            ride_button_state = false;
            switch (trip_state) {
                case OFF: {
                    if (ride_button_state) {
                        trip_state = ON;
                    }
                    break;
                }

                case ON: {
                    if (!ride_button_state) {
                        // Stop trip
                        // TODO: Call function to change displays (format unknown)
                        // TODO: uint32_t get_time_elapsed(void) 
                        // TODO: float get_dist_traveled(void)
                        // TODO: float get_elev_change(void)
                        trip_state = OFF;
                    }
                    break;
                }
            }

            // FSM for headlight
            light_val = sample_light_level(0);
            switch (head_light_state) {
                case OFF: {
                    // If LUX is below threshold
                    if (light_val < HEADLIGHT_THRESHOLD) {
                        // TURN LIGHT ON
                        toggle_headlight();
                        // TODO: toggle_taillight_char();
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 6);
                        head_light_state = ON;
                    } else {
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 7);
                        head_light_state = OFF;
                    }
                    break;
                }

                case ON: {
                    // If LUX is above threshold
                    if (light_val > HEADLIGHT_THRESHOLD) {
                        // TURN LIGHT OFF
                        toggle_headlight();
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 7);
                        // TODO: toggle_taillight_char();
                        head_light_state = OFF;
                    } else {
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 6);
                        head_light_state = ON;
                    }
                    break;
                }
            }

            // FSM for turnlights.
            roll = get_roll_degrees();
            gyro_z = get_gyro_z();
            intended_right = *roll > ANGLE_THRESHOLD || *gyro_z > GYROZ_THRESHOLD;
            intended_left = *roll < -ANGLE_THRESHOLD || *gyro_z < -GYROZ_THRESHOLD;
            // The physical switch for manual turn signals. Overrides automatic signals when active (left or right).
            switch_state_left = nrf_gpio_pin_read(9);
            switch_state_right = nrf_gpio_pin_read(10);
            switch (turn_light_state) {
                case TURN_OFF: {
                    if (!switch_state_right && (switch_state_left || nav_turn_left || intended_left)) {
                        toggle_flash_left();
                        display_set_leftTurn(true);
                        NRF_LOG_INFO("Turning Left");
                        // Turn on left turn signal on back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 2);
                        // Only start timer if automatic
                        if (intended_left) {
                            automatic_turn_left = true;
                            left_turn_timer_id = virtual_timer_start(3000000, cancel_left_turn_timer);
                        }
                        turn_light_state = TURN_LEFT;
                    } else if (switch_state_right || nav_turn_right || intended_right) {
                        toggle_flash_right();
                        display_set_rightTurn(true);
                        NRF_LOG_INFO("Turning Right");
                        // Turn on right turn signal on back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 4);
                        // Only start timer if automatic
                        if (intended_right) {
                            automatic_turn_right = true;
                            right_turn_timer_id = virtual_timer_start(3000000, cancel_right_turn_timer);
                        }
                        turn_light_state = TURN_RIGHT;
                    } else {
                        turn_light_state = TURN_OFF;
                    }
                    break;
                }

                case TURN_LEFT: {
                    if (!switch_state_right && (switch_state_left || nav_turn_left || intended_left)) {
                        // Do nothing
                        turn_light_state = TURN_LEFT;
                    } else if (switch_state_right || nav_turn_right || intended_right) {
                        // First toggle left
                        toggle_flash_left();
                        display_set_leftTurn(false);
                        // Turn off left turn signal on back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 3);
                        cancel_left_turn_timer();

                        // Then toggle right
                        toggle_flash_right();
                        display_set_rightTurn(true);
                        // Turn on right turn signal on back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 4);
                        // Only start timer if automatic
                        if (intended_right) {
                            automatic_turn_right = true;
                            right_turn_timer_id = virtual_timer_start(3000000, cancel_right_turn_timer);
                        }
                        turn_light_state = TURN_RIGHT;
                    } else {
                        if (automatic_turn_left) {
                            // Do nothing
                            turn_light_state = TURN_LEFT;
                        } else {
                            // If no longer turning, toggle lights off
                            toggle_flash_left();
                            display_set_leftTurn(false);
                            // Turn off left turn signal on back module
                            ble_send_backlight_command(m_conn_handle, &m_lbs, 3);
                            turn_light_state = TURN_OFF;
                        }
                    }
                    break;
                }

                case TURN_RIGHT: {
                    if (!switch_state_right && (switch_state_left || nav_turn_left || intended_left)) {
                        // First  toggle right
                        toggle_flash_right();
                        display_set_rightTurn(false);
                        // Turn off right turn signal on back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 5);
                        cancel_right_turn_timer();

                        // Then toggle left
                        toggle_flash_left();
                        display_set_leftTurn(true);
                        NRF_LOG_INFO("Turning Right");
                        // Turn on left turn signal on back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 2);
                        // Only start timer if automatic
                        if (intended_left) {
                            automatic_turn_left = true;
                            left_turn_timer_id = virtual_timer_start(3000000, cancel_left_turn_timer);
                        }
                        turn_light_state = TURN_LEFT;
                    } else if (switch_state_right || nav_turn_right || intended_right) {
                        // Do nothing
                        turn_light_state = TURN_RIGHT;
                    } else {
                        if (automatic_turn_right) {
                            turn_light_state = TURN_RIGHT;
                        } else {
                            // If no longer turning, toggle lights off
                            toggle_flash_right();
                            display_set_rightTurn(false);
                            // Turn off right turn signal on back module
                            ble_send_backlight_command(m_conn_handle, &m_lbs, 5);
                            turn_light_state = TURN_OFF;
                        }
                    }
                    break;
                }
            }

            acceleration = get_linaccel_y();
            // FSM for brake light
            switch (brake_light_state) {
                case OFF: {
                    if (*acceleration <= 0) {
                        // Turn brake light on in back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 0);
                        brake_light_state = ON;
                    }
                    break;
                }

                case ON: {
                    if (*acceleration > 0) {
                        // Turn brake light off in back module
                        ble_send_backlight_command(m_conn_handle, &m_lbs, 1);
                        brake_light_state = OFF;
                    }
                    break;
                }
            }

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
                        if (temp_dist < NAV_AUTOMATIC_TURN_DISTANCE) {
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
