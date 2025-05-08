#include "spaceship.h"
#include "display.h"
#include "controls.h"

void app_main() {
    spi_init();
    adc_init();

    Spaceship ship = {.left_wing_index = 0, .nose_index = 1};
    draw_spaceship(&ship, frame_buffer);

    xTaskCreate(display, "display", 2400, &ship, 2, NULL);
    xTaskCreate()

    while (1) {
        clear_buffer(frame_buffer);
        draw_spaceship(&ship, frame_buffer);
        analog_read(&ship);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
