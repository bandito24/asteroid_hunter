#pragma once
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <driver/adc.h>

#define SER_PIN GPIO_NUM_23
#define SRCLK_PIN GPIO_NUM_18
#define LATCH_PIN GPIO_NUM_5
#define move_x_gpio GPIO_NUM_33
#define move_y_gpio GPIO_NUM_32
#define cannon_gpio GPIO_NUM_22

#define MOVE_THRESHOLD 60
#define ROW_COUNT 8
#define COL_COUNT 8

extern spi_device_handle_t spi;
extern adc_bits_width_t width;
extern adc_atten_t atten;

enum Direction {
    x = 0,
    y = 1,
    num_directions
};
