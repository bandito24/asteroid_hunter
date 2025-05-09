#pragma once
#include <stdint.h>
#include "spaceship.h"
#include "esp_attr.h"
#include "config.h"


void spi_init();
void display(void *pvParameters);
void set_pixel(int8_t col, int8_t row);
void clear_buffer();
void display_loss();
void display_win();
void display_level(uint8_t level);