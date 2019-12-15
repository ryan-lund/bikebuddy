#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "front_peripherals.h"
#include "virtual_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "SSD1306.h"
#include "Adafruit_GFX.h"
#include "buffers.h"

#define FLASH_INTERVALD 800000
int d_left_timer_id = 0;
int d_right_timer_id = 0;
bool _leftTurn = 0;
bool _rightTurn = 0;
bool _leftBS = 0;
bool _rightBS = 0;
direction _dir = NONE;
char* _distwp = "";
char* _street = "";
char* _speed  = "";

nrf_drv_twi_t* _m_twi_master0;
nrf_drv_twi_t* _m_twi_master1;


static uint8_t bufferleft_small[SSD1306_128_32_LCDWIDTH * SSD1306_128_32_LCDHEIGHT / 8];
static uint8_t bufferright_small[SSD1306_128_32_LCDWIDTH * SSD1306_128_32_LCDHEIGHT / 8];

static uint8_t bufferleft_big[SSD1306_128_64_LCDWIDTH * SSD1306_128_64_LCDHEIGHT / 8];
static uint8_t bufferright_big[SSD1306_128_64_LCDWIDTH * SSD1306_128_64_LCDHEIGHT / 8];


///// Draw utilty functions

void drawLeftTurn(uint8_t *buffer)
{
//     void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
// void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_drawRect(20, 0, 40, 15, WHITE);
    Adafruit_GFX_fillRect(20, 0, 40, 15, WHITE);
    Adafruit_GFX_drawTriangle(0, 15, 40, 32, 80, 15, WHITE);
    Adafruit_GFX_fillTriangle(0, 15, 40, 32, 80, 15, WHITE);

}

void drawRightTurn(uint8_t *buffer)
{
//     void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
// void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_drawRect(20, 17, 40, 15, WHITE);
    Adafruit_GFX_fillRect(20, 17, 40, 15, WHITE);
    Adafruit_GFX_drawTriangle(0, 17, 40, 0, 80, 17, WHITE);
    Adafruit_GFX_fillTriangle(0, 17, 40, 0, 80, 17, WHITE);

}


void drawArrow(uint8_t *buffer, direction dir) {
    Adafruit_GFX_init(SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT, SSD1306_drawPixel, buffer);
    if (dir == FRONT) {
        Adafruit_GFX_drawRect(27, 15, 10, 49, WHITE);
        Adafruit_GFX_fillRect(27, 15, 10, 49, WHITE);
        Adafruit_GFX_drawTriangle(0, 15, 32, 0, 64, 15, WHITE);
        Adafruit_GFX_fillTriangle(0, 15, 32, 0, 64, 15, WHITE);
    } else if (dir == LEFT) {
        Adafruit_GFX_drawRect(15, 37, 17, 10, WHITE);
        Adafruit_GFX_fillRect(15, 37, 17, 10, WHITE);
        Adafruit_GFX_drawTriangle(0, 32, 15, 0, 15, 64, WHITE);
        Adafruit_GFX_fillTriangle(0, 32, 15, 0, 15, 64, WHITE);
    } else if (dir == RIGHT) {
        Adafruit_GFX_drawRect(0, 37, 17, 10, WHITE);
        Adafruit_GFX_fillRect(0, 37, 17, 10, WHITE);
        Adafruit_GFX_drawTriangle(32, 32, 17, 0, 17, 64, WHITE);
        Adafruit_GFX_drawTriangle(32, 32, 17, 0, 17, 64, WHITE);
    } else if (dir == ARRIVE) {
        Adafruit_GFX_drawRect(0, 37, 17, 10, WHITE);
    }
}

void drawDistWP(uint8_t *buffer, char* dist) {
    Adafruit_GFX_init(SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_setCursor(50,40);
    Adafruit_GFX_setTextColor(WHITE, WHITE);
    Adafruit_GFX_setTextWrap(false);
    Adafruit_GFX_setTextSize(2);
    char * t; // first copy the pointer to not change the original
    for (t = dist; *t != '\0'; t++) {
        Adafruit_GFX_write(t[0]);
    }
}

void drawSpeed(uint8_t *buffer, char* speed) {
    Adafruit_GFX_init(SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_setCursor(0,45);
    Adafruit_GFX_setTextColor(WHITE, WHITE);
    Adafruit_GFX_setTextWrap(false);
    Adafruit_GFX_setTextSize(2);
    char * t; // first copy the pointer to not change the original
    for (t = speed; *t != '\0'; t++) {
        Adafruit_GFX_write(t[0]);
    }
}

void drawStreet(uint8_t *buffer, char* street) {
    Adafruit_GFX_init(SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_setCursor(0,0);
    Adafruit_GFX_setTextColor(WHITE, WHITE);
    Adafruit_GFX_setTextWrap(false);
    Adafruit_GFX_setTextSize(2);
    char * t; // first copy the pointer to not change the original
    for (t = street; *t != '\0'; t++) {
        Adafruit_GFX_write(t[0]);
    }
}

void drawLeftBig() {
    // NRF_LOG_INFO("DRAWING LEFT");
    SSD1306_clearDisplay(bufferleft_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    drawArrow(bufferleft_big, _dir);

    if (_distwp[0] != '\0') {
    
        drawDistWP(bufferleft_big, _distwp);
    }
    SSD1306_display(_m_twi_master0, SSD1306_I2C_ADDRESS2, bufferleft_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
}


void drawRightBig() {
    SSD1306_clearDisplay(bufferright_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    if (_speed[0] != '\0') {
        drawSpeed(bufferright_big, _speed);
    }
    if (_street[0] != '\0') {
        drawStreet(bufferright_big, _street);
    }
    Adafruit_GFX_drawLine(0, 32, 128, 32, WHITE);
    SSD1306_display(_m_twi_master1, SSD1306_I2C_ADDRESS2, bufferright_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
}

void display_set_direction(direction dir) {
    _dir = dir;
    drawLeftBig();
}
void display_set_disttowp(char* dist) {
    _distwp = malloc(strlen(dist) + 1);
    strcpy(_distwp, dist);
    drawLeftBig();
}


void display_set_speed(char* speed) {
    _speed = malloc(strlen(speed) + 1);
    strcpy(_speed, speed);
    drawRightBig();
}


void display_set_street(char* street) {
    _street = malloc(strlen(street) + 1);
    strcpy(_street, street);
    drawRightBig();
}

void drawBS(uint8_t *buffer)
{
//     void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
// void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

    Adafruit_GFX_init(SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT, SSD1306_drawPixel, buffer);
    Adafruit_GFX_drawTriangle(96, 16, 128, 32, 128, 0, WHITE);
    Adafruit_GFX_fillTriangle(96, 16, 128, 32, 128, 0, WHITE);

}


void drawLeftSmall() {
    // NRF_LOG_INFO("DRAWING LEFT");
    SSD1306_clearDisplay(bufferleft_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    if (_leftTurn) {
        drawLeftTurn(bufferleft_small);
    }

    if (_leftBS) {
        drawBS(bufferleft_small);
    }
    SSD1306_display(_m_twi_master0, SSD1306_I2C_ADDRESS, bufferleft_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
}

void drawRightSmall() {
    SSD1306_clearDisplay(bufferright_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    if (_rightTurn) {
        drawRightTurn(bufferright_small);
    }

    if (_rightBS) {
        drawBS(bufferright_small);
    }
    SSD1306_display(_m_twi_master1, SSD1306_I2C_ADDRESS, bufferright_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
}






void display_toggle_left(void) {
    _leftTurn = !(_leftTurn);
    drawLeftSmall();
}


void display_toggle_flash_left(void) {
	NRF_LOG_INFO("STARTING TIMER");
	if (!d_left_timer_id || _leftTurn) {
		d_left_timer_id = virtual_timer_start_repeated(FLASH_INTERVALD, display_toggle_left);
	} else {
		virtual_timer_cancel(d_left_timer_id);
		d_left_timer_id = 0;
	}
}

void display_set_leftTurn(bool set_val) {
    _leftTurn = set_val;
    display_toggle_flash_left();
}

void display_set_leftBS(bool set_val) {
    _leftBS = set_val;
    drawLeftSmall();
}

/// right small


void display_toggle_right(void) {
    _rightTurn = !(_rightTurn);
    drawRightSmall();
}


void display_toggle_flash_right(void) {
	NRF_LOG_INFO("STARTING RIGHT TURN TIMER");
	if (!d_right_timer_id || _rightTurn) {
		d_right_timer_id = virtual_timer_start_repeated(FLASH_INTERVALD, display_toggle_right);
	} else {
		virtual_timer_cancel(d_right_timer_id);
		d_right_timer_id = 0;
	}
}

void display_set_rightTurn(bool set_val) {
    _rightTurn = set_val;
    display_toggle_flash_right();
}

void display_set_rightBS(bool set_val) {
    _rightBS = set_val;
    drawRightSmall();
}








void init_peripherals(nrf_drv_twi_t* m_twi_master0, nrf_drv_twi_t* m_twi_master1) {
    NRF_LOG_INFO("INIT CALLED");
    _m_twi_master0 = m_twi_master0;
    _m_twi_master1 = m_twi_master1;
    SSD1306_clearDisplay(bufferleft_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    SSD1306_clearDisplay(bufferright_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    SSD1306_clearDisplay(bufferleft_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    SSD1306_clearDisplay(bufferright_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    NRF_LOG_INFO("DISPLAYS CLEARED");
    SSD1306_begin(_m_twi_master0, SSD1306_I2C_ADDRESS, SSD1306_SWITCHCAPVCC, false, SSD1306_128_32, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    SSD1306_begin(_m_twi_master0, SSD1306_I2C_ADDRESS2, SSD1306_SWITCHCAPVCC, false, SSD1306_128_64, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    NRF_LOG_INFO("twi0 begun");
    SSD1306_begin(_m_twi_master1, SSD1306_I2C_ADDRESS, SSD1306_SWITCHCAPVCC, false, SSD1306_128_32, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    SSD1306_begin(_m_twi_master1, SSD1306_I2C_ADDRESS2, SSD1306_SWITCHCAPVCC, false, SSD1306_128_64, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    NRF_LOG_INFO("twi1 begun");
    SSD1306_display(_m_twi_master0, SSD1306_I2C_ADDRESS, bufferleft_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    SSD1306_display(_m_twi_master0, SSD1306_I2C_ADDRESS2, bufferleft_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    NRF_LOG_INFO("twi0 display cleared");
    SSD1306_display(_m_twi_master1, SSD1306_I2C_ADDRESS, bufferright_small, SSD1306_128_32_LCDWIDTH, SSD1306_128_32_LCDHEIGHT);
    SSD1306_display(_m_twi_master1, SSD1306_I2C_ADDRESS2, bufferright_big, SSD1306_128_64_LCDWIDTH, SSD1306_128_64_LCDHEIGHT);
    NRF_LOG_INFO("twi1 display cleareed");
    NRF_LOG_INFO("ABOUT TO INIT TIMER");
    NRF_LOG_INFO("TIMER INITIALIZED");
}
