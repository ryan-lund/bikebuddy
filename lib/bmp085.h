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

/*
//settings struct
typedef struct BMP_SETTINGS {
  BMP_TypeDef *bmp;
  uint8_t       OSS;
} BMP_SETTINGS;
*/

//public
void bmp085_init(const nrf_twi_mngr_t* instance);
void bmp085_run(void);
float bmp085_Conv_to_F(float Temp_C);
float bmp085_getTemperature(void);
uint32_t bmp085_getPressure(void);
float bmp085_get_altitude_meters(void);
float bmp085_get_altitude_feet(void);
void bmp085_print(void);
void bmp085_reset_counter(void);
uint8_t bmp085_getHz(void);