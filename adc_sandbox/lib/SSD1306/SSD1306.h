/*********************************************************************
This is a port of the Adafruit SSD1306 driver to Nordic nRF52832.

Currently only I2C supported.

Original:

https://github.com/adafruit/Adafruit_SSD1306

electronut.in
*********************************************************************/


#ifndef _SSD1306_H_
#define _SSD1306_H_

#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "nrf_drv_twi.h"

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1306_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D
#define SSD1306_I2C_ADDRESS2  0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

#define SSD1306_128_64 0
#define SSD1306_128_32 1

#define SSD1306_128_64_LCDWIDTH                  128
#define SSD1306_128_64_LCDHEIGHT                 64
#define SSD1306_128_32_LCDWIDTH                  128
#define SSD1306_128_32_LCDHEIGHT                 32


#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

int8_t _vccstate;

void SSD1306_begin(nrf_drv_twi_t* m_twi_master, uint8_t i2caddr, uint8_t vccstate, bool reset, uint8_t displaytype, uint8_t width, uint8_t height);
void SSD1306_command(nrf_drv_twi_t* m_twi_master, uint8_t i2caddr, uint8_t c);

void SSD1306_clearDisplay(uint8_t *buffer, uint8_t width, uint8_t height);
void SSD1306_display(nrf_drv_twi_t* m_twi_master, uint8_t i2caddr, uint8_t *buffer, uint8_t width, uint8_t height);

void SSD1306_drawPixel(uint8_t *buffer, int16_t x, int16_t y, uint16_t color, uint8_t width, uint8_t height);

static const uint8_t el_logo[SSD1306_128_64_LCDHEIGHT * SSD1306_128_64_LCDWIDTH / 8] =
{
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x80,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7,0x87,0x87,0x80,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x5,0x86,0x86,0xc0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7,0x3,0x83,0x80,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x1,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x1,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x83,0x6,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc3,0xc,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x63,0x18,0x0,0x0,0x0,0x0,0x40,
  0x7,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x33,0x30,0x0,0x0,0x0,0x1,0xe0,
  0x4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x33,0x30,0x1f,0xff,0xff,0xff,0xa0,
  0x7,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x30,0x31,0x10,0x30,0x0,0x0,0x1,0xe0,
  0x3,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x0,0x0,0x20,0x0,0x0,0x0,0xc0,
  0x0,0x0,0x10,0x0,0x0,0x0,0x2,0x0,0x18,0x3f,0xf8,0x60,0x0,0x0,0x3,0x0,
  0x0,0x0,0x18,0x0,0x0,0x0,0x3,0x0,0xe,0x40,0x9,0xc0,0x0,0x0,0x3,0x0,
  0x3,0xf0,0x18,0x1f,0xc0,0x7f,0x3,0xc1,0xe6,0x4f,0xc9,0xdf,0xe0,0x61,0x83,0xc0,
  0x7,0xf8,0x18,0x3f,0xe0,0xff,0x3,0xc1,0xf0,0x5f,0xe8,0x1f,0xf0,0xe1,0x83,0xe0,
  0x7,0xfc,0x18,0x3f,0xe0,0xff,0x3,0x81,0xe0,0x7f,0xe8,0x1f,0xf0,0xe1,0x83,0xc0,
  0x7,0xfc,0x18,0x3f,0xe0,0xe0,0x3,0x1,0x80,0x58,0x68,0x18,0x70,0xe1,0x83,0x0,
  0x7,0xfc,0x18,0x3f,0xe0,0xe0,0x3,0x1,0x80,0x58,0x6c,0x18,0x70,0xe1,0x83,0x0,
  0x7,0x0,0x18,0x38,0x0,0xe0,0x3,0x1,0x80,0x58,0x68,0x18,0x70,0xe1,0x83,0x0,
  0x7,0xf8,0x18,0x3f,0xc0,0xff,0x83,0xc1,0x80,0x5f,0xe8,0x18,0x70,0xff,0x83,0xc0,
  0x7,0xf8,0x18,0x1f,0xc0,0x7f,0x83,0xc1,0x86,0x5f,0xe9,0x98,0x70,0x7f,0x83,0xe0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0x40,0x9,0xc0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x18,0x7f,0xf8,0x60,0x0,0x0,0x0,0x0,
  0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x3f,0xf0,0x20,0x0,0x0,0x0,0x0,
  0x7,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x30,0x0,0x0,0x30,0x0,0x0,0x0,0xe0,
  0x5,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x33,0x30,0x1f,0xff,0xff,0xff,0xa0,
  0x7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0x33,0x30,0xf,0xff,0xff,0xff,0xe0,
  0x3,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x33,0x18,0x0,0x0,0x0,0x0,0xc0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x63,0xc,0x0,0x3,0x0,0xc,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc3,0x6,0x4,0x3,0xf,0x9e,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x83,0x3,0x4,0x7,0xd,0xb0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x1,0x4,0x4,0x8c,0x90,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x1,0x4,0xd,0xcf,0x9c,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x84,0xf,0xcf,0xc6,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7,0x87,0x83,0x84,0x17,0xac,0xc2,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x5,0x86,0x86,0xc6,0x93,0x2d,0xf6,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7,0x3,0x83,0x87,0xdf,0xef,0x9e,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};

#endif /* _SSD1306_H_ */
