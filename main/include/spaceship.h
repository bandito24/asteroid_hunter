#pragma once
#include <stdint.h>

typedef struct {
    uint8_t left_wing_index;
    uint8_t nose_index;
} Spaceship;

void move_x(int8_t direction, Spaceship *ship);
void move_y(int8_t direction, Spaceship *ship);
void draw_spaceship(Spaceship *ship, int8_t *frame_buffer);