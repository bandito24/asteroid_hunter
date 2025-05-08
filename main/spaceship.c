#include "spaceship.h"

#define MAX_BULLETS 10
Bullet bullets[MAX_BULLETS] = {0};
uint8_t ship_width = 3;
uint8_t ship_height = 2;

extern void set_pixel(int8_t col, int8_t row, int8_t *frame_buffer); // from display.c

void move_x(int8_t direction, Spaceship *ship)
{
    int8_t new_left_pos = ship->left_wing_index + direction;
    if (new_left_pos >= 0 && new_left_pos <= 5)
        ship->left_wing_index = (uint8_t)new_left_pos;
}

void move_y(int8_t direction, Spaceship *ship)
{
    int8_t new_nose_pos = ship->nose_index + direction;
    if (new_nose_pos >= 1 && new_nose_pos <= 7)
        ship->nose_index = (uint8_t)new_nose_pos;
}

void draw_spaceship(Spaceship *ship, int8_t *frame_buffer)
{
    for (int width = 0; width < ship_width; width++)
    {
        set_pixel(ship->left_wing_index + width, ship->nose_index - 1, frame_buffer);
    }
    set_pixel(ship->left_wing_index + 1, ship->nose_index, frame_buffer);
}

void bullet_task(void *pvParameters)
{
    BulletTaskContext *ctx = (BulletTaskContext *)pvParameters;
    while (1)
    {
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (ctx->bullets[i].active)
            {
                if (ctx->bullets[i].y_pos <= COL_COUNT - 2)
                {
                    ctx->bullets[i].y_pos += 1;
                    set_pixel(ctx->bullets[i].y_pos, ctx->bullets[i].x_pos, ctx->frame_buffer);
                } else{
                    ctx->bullets[i].active = false;
                }
            }
        }
    }
}

void shoot_bullet(Spaceship *ship, Bullet *bullets){
    Bullet next_bullet = {
        .x_pos = ship->left_wing_index + 1,
        .y_pos = ship->nose_index + 1,
        .active = true,
    };
    for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i] = next_bullet;
            }
        }
}