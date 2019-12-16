#include <stdint.h>
#include "nrf_drv_twi.h"

typedef enum {
  FRONT,
  LEFT,
  RIGHT,
  ARRIVE,
  NONE
} direction;


void display_set_leftBS(bool set_val);
void display_set_leftTurn(bool set_val);
void display_set_rightBS(bool set_val);
void display_set_rightTurn(bool set_val);
void display_set_direction(direction dir);
void display_set_disttowp(char* dist);
void display_set_speed(char* speed);
void display_set_street(char* street);

uint8_t get_imu_id(void);
double* get_linaccel_x(void);
double* get_linaccel_y(void);
double* get_linaccel_z(void);

double* get_gyro_z(void);
double* get_roll_degrees(void);

double* get_pitch_degrees(void);

double* get_heading_degrees(void);



void init_peripherals(nrf_drv_twi_t* m_twi_master0, nrf_drv_twi_t* m_twi_master1);