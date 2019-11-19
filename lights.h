#ifndef LIGHTS_H_
#define LIGHTS_H_

#define LEFT_PIN 4
#define RIGHT_PIN 5
#define HEAD_PIN 28
#define TAIL_PIN 29
#define BRAKE_PIN 30

void* init_lights(void*);

void* toggle_left(void*);

void* toggle_right(void*);

void* toggle_headlight(void*);

void* toggle_taillight(void*);

void* toggle_brake(void*);
#endif // LIGHTS_H_