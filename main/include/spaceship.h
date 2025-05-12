#pragma once
#include "config.h"
#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>


#define MAX_BULLETS 10
#define MAX_ASTEROIDS 10
#define ASTEROID_LEVEL_THRESHOLD 5
#define MAX_LEVELS 9
#define STARTING_ASTEROID_RATE_MS 600

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
    Point *cannon_index;
    Point *left_wing_index
} Spaceship;

typedef struct {
    Point dimmensions[4];
    int8_t dimmension_count;
    bool active
} Asteroid;

typedef struct {
    Bullet *bullets;
    SemaphoreHandle_t mutex;
} BulletTaskContext;

typedef struct {
    uint8_t level;
    bool has_lost;
    bool has_won;
    bool idle;
} gameplay_state;

extern Bullet bullets[MAX_BULLETS];
extern BulletTaskContext bullet_task_context;

void move_x(int8_t direction);
void move_y(int8_t direction);
void draw_gameplay();
void shoot_bullet();
void bullet_task(void *pvParameters);
void asteroid_task(void *pvParameters);
void game_play_init();
void draw_spaceship(int8_t left_wing_index, int8_t nose_index);
void reset_spaceship();
void activate_loss();
void activate_win();
