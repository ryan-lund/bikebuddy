#include <stdint.h>
#include "nrf_drv_twi.h"

typedef enum {
  FRONT,
  LEFT,
  RIGHT,
  ARRIVE
} direction;


void display_toggle_flash_left(void);
void display_set_leftBS(bool set_val);
void display_set_leftTurn(bool set_val);
void display_set_rightBS(bool set_val);
void display_set_rightTurn(bool set_val);
void display_set_direction(direction dir);
void display_set_disttowp(char* dist);
void display_set_speed(char* speed);
void display_set_street(char* street);

void init_peripherals(nrf_drv_twi_t* m_twi_master0, nrf_drv_twi_t* m_twi_master1);