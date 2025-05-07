#pragma once
#include <stdint.h>
#include "spaceship.h"
#include "esp_attr.h"
#include "config.h"

extern int8_t frame_buffer[ROW_COUNT];

void spi_init();
void display(void *pvParameters);
void set_pixel(int8_t col, int8_t row, int8_t *frame_buffer);
void clear_buffer(int8_t *frame_buffer);