#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"

#define MAX_BULLETS 10
#define MAX_ASTEROIDS 10


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
    int8_t x_pos;
    int8_t y_pos;
    bool initialized;
} Point;

typedef struct {
    Point dimmensions[4];
    int8_t dimmension_count;
    bool active
} Asteroid;

typedef struct {
    Bullet *bullets;
    SemaphoreHandle_t mutex;
} BulletTaskContext;

extern Bullet bullets[MAX_BULLETS];
extern BulletTaskContext bullet_task_context;

void move_x(int8_t direction);
void move_y(int8_t direction);
void draw_spaceship();
void shoot_bullet();
void bullet_task(void *pvParameters);
void asteroid_task(void *pvParameters);
void game_play_init();
