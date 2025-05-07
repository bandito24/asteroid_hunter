#include "controls.h"
#include "config.h"
// #include "esp_adc_cal.h"

adc1_channel_t analog_channels[num_directions] = {
    [x] = ADC_CHANNEL_4,
    [y] = ADC_CHANNEL_5
};

adc_bits_width_t width = ADC_WIDTH_BIT_12;
adc_atten_t atten = ADC_ATTEN_DB_12;
uint32_t analog_baseline[num_directions];

void adc_init() {
    ESP_ERROR_CHECK(adc1_config_width(width));
    for (int i = 0; i < num_directions; i++)
        ESP_ERROR_CHECK(adc1_config_channel_atten(analog_channels[i], atten));

    const uint8_t num_measurements = 64;
    for (int i = 0; i < num_directions; i++) {
        uint32_t adc_reading = 0;
        for (int j = 0; j < num_measurements; j++)
            adc_reading += adc1_get_raw(analog_channels[i]);
        analog_baseline[i] = adc_reading / num_measurements;
    }
}

void analog_read(Spaceship *ship) {
    uint32_t adc_readings[num_directions];
    for (int i = 0; i < num_directions; i++)
        adc_readings[i] = adc1_get_raw(analog_channels[i]);

    int32_t diffX = adc_readings[x] - analog_baseline[x];
    int32_t diffY = adc_readings[y] - analog_baseline[y];
    if (abs(diffX) >= MOVE_THRESHOLD || abs(diffY) >= MOVE_THRESHOLD) {
        if (abs(diffX) > abs(diffY))
            move_x(diffX < 0 ? -1 : 1, ship);
        else
            move_y(diffY < 0 ? -1 : 1, ship);
    }
}
