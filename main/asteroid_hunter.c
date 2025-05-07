#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/ledc.h"
#include "driver/touch_sensor.h"
#include <rom/ets_sys.h>
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include <stdlib.h>

#define SER_PIN GPIO_NUM_23
#define SRCLK_PIN GPIO_NUM_18
#define LATCH_PIN GPIO_NUM_5
#define move_x_gpio GPIO_NUM_33
#define move_y_gpio GPIO_NUM_32
#define cannon_gpio GPIO_NUM_22
#define MOVE_THRESHOLD 60

spi_device_handle_t spi;
adc_bits_width_t width = ADC_WIDTH_BIT_12;
adc_atten_t atten = ADC_ATTEN_DB_12;

enum
{
  x = 0,
  y = 1,
  num_directions
};
adc1_channel_t analog_channels[num_directions] = {[x] = ADC_CHANNEL_4, [y] = ADC_CHANNEL_5};

typedef struct
{
  uint8_t pos_col[2];
  uint8_t pos_row[2];
  uint8_t left_wing_index;
  uint8_t nose_index
} Spaceship;

void latch()
{
  gpio_set_level(LATCH_PIN, 1);
  ets_delay_us(1);
  gpio_set_level(LATCH_PIN, 0);
}

extern spi_device_handle_t spi;

void spi_init()
{
  spi_bus_config_t buscfg = {
      .mosi_io_num = SER_PIN,
      .sclk_io_num = SRCLK_PIN,
      .miso_io_num = -1,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0};

  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = 1 * 1000 * 1000,
      .mode = 0,
      .spics_io_num = -1,
      .queue_size = 1};

  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
  ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));

  gpio_set_direction(LATCH_PIN, GPIO_MODE_OUTPUT);
}

void move_x(int8_t direction, Spaceship *ship)
{
  int8_t new_left_pos = ship->left_wing_index + direction;
  if (new_left_pos >= 0 && new_left_pos <= 5)
  {
    ship->left_wing_index = (uint8_t)new_left_pos;
    if (direction < 0)
    {
      ship->pos_col[1] = (ship->pos_col[1] << 1);
      ship->pos_col[0] = (ship->pos_col[0] << 1);
    }
    else
    {
      ship->pos_col[1] = (ship->pos_col[1] >> 1);
      ship->pos_col[0] = (ship->pos_col[0] >> 1);
    }
  }
}
void move_y(int8_t direction, Spaceship *ship)
{
  int8_t new_nose_pos = ship->nose_index + direction;
  if (new_nose_pos >= 0 && new_nose_pos <= 6)
  {
    ship->nose_index = (uint8_t)new_nose_pos;
    if (direction < 0)
    {
      ship->pos_row[1] = (ship->pos_row[1] >> 1);
      ship->pos_row[0] = (ship->pos_row[0] >> 1);
    }
    else
    {
      ship->pos_row[1] = (ship->pos_row[1] << 1);
      ship->pos_row[0] = (ship->pos_row[0] << 1);
    }
  }
}

void display(void *pvParameters)
{
  Spaceship *ship = pvParameters;
  while (1)
  {
    for (int i = 0; i < 2; i++)
    {

      spi_transaction_t t = {
          .length = 16,
          .tx_data = {ship->pos_row[i], ~(ship->pos_col[i])},
          .flags = SPI_TRANS_USE_TXDATA};
      spi_device_transmit(spi, &t);
      latch();
    }

    vTaskDelay(pdMS_TO_TICKS(8));
  }
}

uint32_t analog_baseline[num_directions];

void adc_init()
{

  ESP_ERROR_CHECK(adc1_config_width(width));
  for (int i = 0; i < num_directions; i++)
  {
    ESP_ERROR_CHECK(adc1_config_channel_atten(analog_channels[i], atten));
  }
  const uint8_t num_measurements = 64;
  for (int i = 0; i < num_directions; i++)
  {
    uint32_t adc_reading = 0;
    for (int j = 0; j < num_measurements; j++)
    {
      adc_reading += adc1_get_raw((adc1_channel_t)analog_channels[i]);
    }
    analog_baseline[i] = adc_reading / num_measurements;
  }
}
void analog_read(Spaceship *ship)
{
  uint32_t adc_readings[num_directions];
  for (int i = 0; i < num_directions; i++)
  {
    adc_readings[i] = adc1_get_raw((adc1_channel_t)analog_channels[i]);
  }

  int32_t diffX = adc_readings[x] - analog_baseline[x];
  int32_t diffY = adc_readings[y] - analog_baseline[y];
  if (abs(diffX) >= MOVE_THRESHOLD || abs(diffY) >= MOVE_THRESHOLD) {
    if (abs(diffX) > abs(diffY)) {
        move_x(diffX < 0 ? -1 : 1, ship);
    } else {
        move_y(diffY < 0 ? -1 : 1, ship);
    }
}

}



void app_main()
{

  spi_init();
  adc_init();

  Spaceship ship = {
      .pos_col = {0b00001000, 0b00011100},
      .pos_row = {0b00000010, 0b00000001},
      .left_wing_index = 3,
      .nose_index = 0};

  TaskHandle_t display_handle;
  xTaskCreate(display, "display matrix", 2400, &ship, 2, &display_handle);

  while (1)
  {
    analog_read(&ship);
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

