#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"



typedef struct {
    uint8_t left_wing_index;
    uint8_t nose_index;
} Spaceship;

typedef struct {
    uint8_t x_pos;
    uint8_t y_pos;
    bool active;
} Bullet;

typedef struct {
    int8_t *frame_buffer;
    Bullet *bullets;
    SemaphoreHandle_t mutex;
} BulletTaskContext;

void move_x(int8_t direction, Spaceship *ship);
void move_y(int8_t direction, Spaceship *ship);
void draw_spaceship(Spaceship *ship, int8_t *frame_buffer);
void shoot_blaster(Spaceship *ship, int8_t *frame_buffer);