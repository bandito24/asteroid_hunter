#include "spaceship.h"
#include "display.h"
#include "controls.h"




void app_main()
{

    spi_init();
    adc_init();
    game_play_init();

    draw_gameplay();

    xTaskCreate(display, "display", 2400, NULL, 2, NULL);
    xTaskCreate(bullet_task, "bullet_task", 2400, NULL, 1, NULL);
    xTaskCreate(asteroid_task, "asteroid_task", 2400, NULL, 1, NULL);
    while (1)
    {
        clear_buffer();
        draw_gameplay();
        analog_read();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
