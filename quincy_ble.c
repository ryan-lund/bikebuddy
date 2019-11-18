// NOTE: WORKING FILE FOR BLE COMMS
// BLE Nav Service Template

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_twi_mngr.h"
#include "nrf_gpio.h"
#include "display.h"

#include "simple_ble.h"
#include "buckler.h"

#include "max44009.h"

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
        // c0:98:e5:49:xx:xx
        .platform_id       = 0x49,    // used as 4th octect in device BLE address
        .device_id         = 0xE149, // TODO: replace with your lab bench number
        .adv_name          = "Bike Buddy", // used in advertisements if there is room
        .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

// 3db02924-b2a6-4d47-be1f-0f90ad62a048
static simple_ble_service_t map_service = {{
    .uuid128 = {0x48,0xA0,0x62,0xAD,0x90,0x0F,0x1F,0xBE,
                0x47,0x4D,0xA6,0xB2,0x24,0x29,0xB0,0x3D}
}};

static simple_ble_char_t nav_char = {.uuid16 = 0x2925};
static simple_ble_char_t dst_char = {.uuid16 = 0x2926};
static simple_ble_char_t loc_char = {.uuid16 = 0x2927};

static bool map_state = true;
static char nav_display[64];
static char dst_display[64];
static char loc_display[64];

/*******************************************************************************
 *   State for this application
 ******************************************************************************/
// Main application state
simple_ble_app_t* simple_ble_app;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
    if (simple_ble_is_char_event(p_ble_evt, &nav_char)) {
      display_write(nav_display, DISPLAY_LINE_0);
      printf("1 %s!\n", nav_display);
    }
    if (simple_ble_is_char_event(p_ble_evt, &dst_char)) {
      display_write(dst_display, DISPLAY_LINE_1);
      printf("2 %s!\n", dst_display);
    }
    if (simple_ble_is_char_event(p_ble_evt, &loc_char)) {
      display_write(loc_display, DISPLAY_LINE_1);
      printf("3 %s!\n", loc_display);
    }
}

int main(void) {

  // Initialize

  // initialize display
  nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
  nrf_drv_spi_config_t spi_config = {
    .sck_pin = BUCKLER_LCD_SCLK,
    .mosi_pin = BUCKLER_LCD_MOSI,
    .miso_pin = BUCKLER_LCD_MISO,
    .ss_pin = BUCKLER_LCD_CS,
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0,
    .frequency = NRF_DRV_SPI_FREQ_4M,
    .mode = NRF_DRV_SPI_MODE_2,
    .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  };

  ret_code_t error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(error_code);
  display_init(&spi_instance);
  display_write("Bike Buddy Ready", DISPLAY_LINE_0);
  printf("Display initialized!\n");

  // Setup LED GPIO
  nrf_gpio_cfg_output(BUCKLER_LED0);

  // Setup BLE
  simple_ble_app = simple_ble_init(&ble_config);

  simple_ble_add_service(&map_service);

  simple_ble_add_characteristic(1, 1, 0, 0,
      sizeof(nav_display), (uint8_t*)&nav_display,
      &map_service, &nav_char);

  simple_ble_add_characteristic(1, 1, 0, 0,
    sizeof(dst_display), (uint8_t*)&dst_display,
    &map_service, &dst_char);

  simple_ble_add_characteristic(1, 1, 0, 0,
    sizeof(loc_display), (uint8_t*)&loc_display,
    &map_service, &dst_display);

  // Start Advertising
  simple_ble_adv_only_name();

  while(1) {
    power_manage();
  }
}

