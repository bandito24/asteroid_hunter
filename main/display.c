#include "display.h"
#include "config.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include <string.h>

spi_device_handle_t spi;
static DRAM_ATTR int8_t frame_buffer[ROW_COUNT] = {0x00};

void latch()
{
    gpio_set_level(LATCH_PIN, 1);
    esp_rom_delay_us(1);
    gpio_set_level(LATCH_PIN, 0);
}

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

void display(void *pvParameters)
{
    while (1)
    {
        for (int row = 0; row < ROW_COUNT; row++)
        {
            spi_transaction_t t = {
                .length = 16,
                .tx_data = {1 << row, ~(frame_buffer[row])},
                .flags = SPI_TRANS_USE_TXDATA};
            spi_device_transmit(spi, &t);
            latch();
        }
        vTaskDelay(pdMS_TO_TICKS(8));
    }
}

void set_pixel(int8_t col, int8_t row)
{
    if (row < 0 || row >= ROW_COUNT || col < 0 || col >= COL_COUNT)
        return;
    frame_buffer[row] |= (1 << (COL_COUNT - 1 - col));
}

void clear_buffer()
{
    memset(frame_buffer, 0x00, ROW_COUNT);
}