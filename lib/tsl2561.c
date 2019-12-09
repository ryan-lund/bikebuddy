#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "tsl2561.h"

static tsl2561IntegrationTime_t _tsl2561IntegrationTime = TSL2561_INTEGRATIONTIME_402MS;
static tsl2561Gain_t _tsl2561Gain = TSL2561_GAIN_0X;

/* Buffer for samples read from lux sensor. */
static uint8_t broadband[2];
static uint8_t ir[2];

static void write_i2c(uint8_t address, uint8_t* reg) {
	err_code = nrf_drv_twi_tx(&m_twi, TSL2561_ADDR, reg, sizeof(reg), false);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false);
}

static void read_i2c(uint8_t address, uint8_t* reg, uint8_t* dest) {
	write_i2c(address, reg) // send read command to device
	m_xfer_done = false;
    /* Read 2 byts from the specified address */
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, TSL2561_ADDR, dest, sizeof(dest));
    APP_ERROR_CHECK(err_code);
}

static void tsl2561_enable(void) {
	ret_code_t err_code;

    // enable device by setting the control bit to 0x03
    uint8_t reg[2] = {TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON};
    write_i2c(TSL2561_ADDR, reg);
}

static void tsl2561_disable(void) {
	ret_code_t err_code;

    // disable device by setting the control bit to 0x03
    uint8_t reg[2] = {TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWEROFF};
    write_i2c(TSL2561_ADDR, reg);
}

static void tsl2561_set_timing(tsl2561IntegrationTime_t integration, tsl2561Gain_t gain) {
	tsl2561_enable();

	uint8_t reg[2] = {TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING, integration | gain};
	write_i2c(TSL2561_ADDR, reg);

	_tsl2561IntegrationTime = integration;
	_tsl2561Gain = gain;

	tsl2561_disable();
}

static void tsl2561_get_luminosity(uint8_t *broadband, uint8_t *ir) {
	tsl2561_enable();

	switch (_tsl2561IntegrationTime) {
		case TSL2561_INTEGRATIONTIME_13MS:
			nrf_delay_ms(14);
			break;
		case TSL2561_INTEGRATIONTIME_101MS:
			nrf_delay_ms(102);
			break;
		default:
			nrf_delay_ms(400);
			break;
	}

	reg[2] = {TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN0_LOW, TSL2561_ADDR | TSL2561_READBIT};
	read_i2c(TSL2561_ADDR, reg, broadband);

	reg[2] = {TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN1_LOW, TSL2561_ADDR | TSL2561_READBIT};
	read_i2c(TSL2561_ADDR, reg, ir);

	tsl2561_disable();
}

static uint32_t tsl2561_calculate_lux(uint16_t ch0, uint16_t ch1)
{
  unsigned long chScale;
  unsigned long channel1;
  unsigned long channel0;

  switch (_tsl2561IntegrationTime)
  {
    case TSL2561_INTEGRATIONTIME_13MS:
      chScale = TSL2561_LUX_CHSCALE_TINT0;
      break;
    case TSL2561_INTEGRATIONTIME_101MS:
      chScale = TSL2561_LUX_CHSCALE_TINT1;
      break;
    default: // No scaling ... integration time = 402ms
      chScale = (1 << TSL2561_LUX_CHSCALE);
      break;
  }

  // Scale for gain (1x or 16x)
  if (!_tsl2561Gain) chScale = chScale << 4;

  // scale the channel values
  channel0 = (ch0 * chScale) >> TSL2561_LUX_CHSCALE;
  channel1 = (ch1 * chScale) >> TSL2561_LUX_CHSCALE;

  // find the ratio of the channel values (Channel1/Channel0)
  unsigned long ratio1 = 0;
  if (channel0 != 0) ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE+1)) / channel0;

  // round the ratio value
  unsigned long ratio = (ratio1 + 1) >> 1;

  unsigned int b, m;

#ifdef TSL2561_PACKAGE_CS
  if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1C))
    {b=TSL2561_LUX_B1C; m=TSL2561_LUX_M1C;}
  else if (ratio <= TSL2561_LUX_K2C)
    {b=TSL2561_LUX_B2C; m=TSL2561_LUX_M2C;}
  else if (ratio <= TSL2561_LUX_K3C)
    {b=TSL2561_LUX_B3C; m=TSL2561_LUX_M3C;}
  else if (ratio <= TSL2561_LUX_K4C)
    {b=TSL2561_LUX_B4C; m=TSL2561_LUX_M4C;}
  else if (ratio <= TSL2561_LUX_K5C)
    {b=TSL2561_LUX_B5C; m=TSL2561_LUX_M5C;}
  else if (ratio <= TSL2561_LUX_K6C)
    {b=TSL2561_LUX_B6C; m=TSL2561_LUX_M6C;}
  else if (ratio <= TSL2561_LUX_K7C)
    {b=TSL2561_LUX_B7C; m=TSL2561_LUX_M7C;}
  else if (ratio > TSL2561_LUX_K8C)
    {b=TSL2561_LUX_B8C; m=TSL2561_LUX_M8C;}
#else
  if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1T))
    {b=TSL2561_LUX_B1T; m=TSL2561_LUX_M1T;}
  else if (ratio <= TSL2561_LUX_K2T)
    {b=TSL2561_LUX_B2T; m=TSL2561_LUX_M2T;}
  else if (ratio <= TSL2561_LUX_K3T)
    {b=TSL2561_LUX_B3T; m=TSL2561_LUX_M3T;}
  else if (ratio <= TSL2561_LUX_K4T)
    {b=TSL2561_LUX_B4T; m=TSL2561_LUX_M4T;}
  else if (ratio <= TSL2561_LUX_K5T)
    {b=TSL2561_LUX_B5T; m=TSL2561_LUX_M5T;}
  else if (ratio <= TSL2561_LUX_K6T)
    {b=TSL2561_LUX_B6T; m=TSL2561_LUX_M6T;}
  else if (ratio <= TSL2561_LUX_K7T)
    {b=TSL2561_LUX_B7T; m=TSL2561_LUX_M7T;}
  else if (ratio > TSL2561_LUX_K8T)
    {b=TSL2561_LUX_B8T; m=TSL2561_LUX_M8T;}
#endif

  unsigned long temp;
  temp = ((channel0 * b) - (channel1 * m));

  // do not allow negative lux value
  if (temp < 0) temp = 0;

  // round lsb (2^(LUX_SCALE-1))
  temp += (1 << (TSL2561_LUX_LUXSCALE-1));

  // strip off fractional portion
  uint32_t lux = temp >> TSL2561_LUX_LUXSCALE;

  // Signal I2C had no errors
  return lux;
}

void tsl2561_init(void) {
	tsl2561_enable();

	tsl2561_set_timing(_tsl2561IntegrationTime, _tsl2561Gain);

	tsl2561_disable();
}

uint32_t tsl2561_get_lux(void) {
	tsl2561_get_luminosity(broadband, ir);
	uint16_t broadband_extended = (broadband[1] << 8 | broadband[0]);
	uint16_t ir_extended = (ir[1] << 8 | ir[0]);
	return tsl2561_get_lux(broadband_extended, ir_extended);
}