/**
  * @file bmp085.h
  * @brief  A Driver for the BMP085 pressure sensor for the STM32-F4 ARM Discovery Board, modified to work wiht nrf52
  * @author Eason Smith modified by Ryan Lund
  * @date 12/23/2015
  *
  */



#pragma once
#include <stdint.h>
#include "nrf_twi_mngr.h"

#define BMP085_ADDRESS 0x77
#define BMP085_CONTROL_REGISTER 0xF4
#define BMP085_TEMPERATURE 0x2E
#define BMP085_PRESSURE 0xF4
#define BMP085_OSS 3

//public
void bmp085_init(const nrf_twi_mngr_t* instance);
float bmp085_Conv_to_F(float Temp_C);
float bmp085_getTemperature(void);
uint32_t bmp085_getPressure(void);
float bmp085_get_altitude_meters(void);
float bmp085_get_altitude_feet(void);