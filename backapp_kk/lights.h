#ifndef LIGHTS_H_
#define LIGHTS_H_

#define LEFT_PIN 30
#define RIGHT_PIN 28
#define TAIL_PIN 29
#define BRAKE_PIN 4
#define FLASH_INTERVAL 500000

void* init_lights(void);

void* start_flash_left(void);

void* stop_flash_left(void);

void* start_flash_right(void);

void* stop_flash_right(void);

void* taillight_on(void);

void* taillight_off(void);

void* brakelight_on(void);

void* brakelight_off(void);
#endif // LIGHTS_H_