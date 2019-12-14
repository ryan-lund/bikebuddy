/*********************************************************************
This is a port of the Adafruit SSD1306 driver to Nordic nRF52832.

Currently only I2C supported.

Original:

https://github.com/adafruit/Adafruit_SSD1306

electronut.in
*********************************************************************/

#include <stdlib.h>
#include <string.h>
#include "nrf_twi_mngr.h" 
#include "app_error.h"

#include "SSD1306_twimgr.h"


#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }



static int width() {return SSD1306_LCDWIDTH;}
static int height() {return SSD1306_LCDHEIGHT;}
static int getRotation() {return 0;}
int WIDTH = SSD1306_LCDWIDTH;
int HEIGHT = SSD1306_LCDHEIGHT;

// the most basic function, set a single pixel
void SSD1306_drawPixel(uint8_t *buffer, int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    ssd1306_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    ssd1306_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // x is which column
    int index = x+ (y/8)*SSD1306_LCDWIDTH;
    //printf("index = %d\n", index);

    switch (color)
    {
      case WHITE:   buffer[index] |=  (1 << (y&7)); break;
      case BLACK:   buffer[index] &= ~(1 << (y&7)); break;
      case INVERSE: buffer[index] ^=  (1 << (y&7)); break;
    }

}


void SSD1306_begin(const nrf_twi_mngr_t* twi_mngr_instance, uint8_t i2caddr, uint8_t vccstate, bool reset) {
  // Init sequence
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_DISPLAYOFF);                    // 0xAE
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  SSD1306_command(twi_mngr_instance, i2caddr, 0x80);                                  // the suggested ratio 0x80

  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETMULTIPLEX);                  // 0xA8
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_LCDHEIGHT - 1);

  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETDISPLAYOFFSET);              // 0xD3
  SSD1306_command(twi_mngr_instance, i2caddr, 0x0);                                   // no offset
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETSTARTLINE | 0x0);            // line #0
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_CHARGEPUMP);                    // 0x8D
  if (vccstate == SSD1306_EXTERNALVCC)
    { SSD1306_command(twi_mngr_instance, i2caddr, 0x10); }
  else
    { SSD1306_command(twi_mngr_instance, i2caddr, 0x14); }
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_MEMORYMODE);                    // 0x20
  SSD1306_command(twi_mngr_instance, i2caddr, 0x00);                                  // 0x0 act like ks0108
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SEGREMAP | 0x1);
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_COMSCANDEC);

 #if defined SSD1306_128_32
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETCOMPINS);                    // 0xDA
  SSD1306_command(twi_mngr_instance, i2caddr, 0x02);
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETCONTRAST);                   // 0x81
  SSD1306_command(twi_mngr_instance, i2caddr, 0x8F);

#elif defined SSD1306_128_64
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETCOMPINS);                    // 0xDA
  SSD1306_command(twi_mngr_instance, i2caddr, 0x12);
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { SSD1306_command(twi_mngr_instance, i2caddr, 0x9F); }
  else
    { SSD1306_command(twi_mngr_instance, i2caddr, 0xCF); }

#elif defined SSD1306_96_16
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETCOMPINS);                    // 0xDA
  SSD1306_command(twi_mngr_instance, i2caddr, 0x2);   //ada x12
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { SSD1306_command(twi_mngr_instance, i2caddr, 0x10); }
  else
    { SSD1306_command(twi_mngr_instance, i2caddr, 0xAF); }

#endif

  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC)
    { SSD1306_command(twi_mngr_instance, i2caddr, 0x22); }
  else
    { SSD1306_command(twi_mngr_instance, i2caddr, 0xF1); }
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_SETVCOMDETECT);                 // 0xDB
  SSD1306_command(twi_mngr_instance, i2caddr, 0x40);
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_NORMALDISPLAY);                 // 0xA6

  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_DEACTIVATE_SCROLL);

  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_DISPLAYON);//--turn on oled panel
}


void SSD1306_command(const nrf_twi_mngr_t* twi_mngr_instance, uint8_t i2caddr, uint8_t c)
{
  ret_code_t ret;
  uint8_t data[] = {0x00, c};
  ret = nrf_drv_twi_tx(&twi_mngr_instance, i2caddr, data, 2, false);
  APP_ERROR_CHECK(ret);
}



void SSD1306_display(const nrf_twi_mngr_t* twi_mngr_instance, uint8_t i2caddr, uint8_t *buffer) {
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_COLUMNADDR);
  SSD1306_command(twi_mngr_instance, i2caddr, 0);   // Column start address (0 = reset)
  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  SSD1306_command(twi_mngr_instance, i2caddr, SSD1306_PAGEADDR);
  SSD1306_command(twi_mngr_instance, i2caddr, 0); // Page start address (0 = reset)
  #if SSD1306_LCDHEIGHT == 64
    SSD1306_command(twi_mngr_instance, i2caddr, 7); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 32
    SSD1306_command(twi_mngr_instance, i2caddr, 3); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 16
    SSD1306_command(twi_mngr_instance, i2caddr, 1); // Page end address
  #endif


    // save I2C bitrate
#ifdef TWBR
    uint8_t twbrbackup = TWBR;
    TWBR = 12; // upgrade to 400KHz!
#endif

    //Serial.println(TWBR, DEC);
    //Serial.println(TWSR & 0x3, DEC);

    // I2C
    for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
      uint8_t tmpBuf[17];
      // SSD1306_SETSTARTLINE
      tmpBuf[0] = 0x40;
      // data
      for (uint8_t j = 0; j < 16; j++) {
        tmpBuf[j+1] = buffer[i];
        i++;
      }
      i--;

      ret_code_t ret;
      ret = nrf_drv_twi_tx(&twi_mngr_instance, i2caddr, tmpBuf, sizeof(tmpBuf), false);
      APP_ERROR_CHECK(ret);
    }

#ifdef TWBR
    TWBR = twbrbackup;
#endif

}

// clear everything
void SSD1306_clearDisplay(uint8_t *buffer) {
  memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}

