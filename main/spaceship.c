#include "spaceship.h"

Bullet bullets[MAX_BULLETS] = {0};
static Asteroid asteroids[MAX_ASTEROIDS] = {0};

uint8_t ship_width = 3;
uint8_t ship_height = 2;
// gpio_isr_handle_t bullet_intr_handle;
static Spaceship ship;
volatile static bool bullet_fired = false;

void IRAM_ATTR bullet_interrupt()
{
    bullet_fired = true;
}

void game_play_init()
{

    ship.left_wing_index = 0;
    ship.nose_index = 1;

    const gpio_config_t bullet_btn_conf = {
        .pin_bit_mask = 1ULL << bullet_gpio,
        .intr_type = GPIO_INTR_NEGEDGE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_INPUT};
    ESP_ERROR_CHECK(gpio_config(&bullet_btn_conf));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(bullet_gpio, bullet_interrupt, NULL));
}

void shoot_bullet()
{
    Bullet next_bullet = {
        .x_pos = ship.left_wing_index + 1,
        .y_pos = ship.nose_index,
        .active = true,
    };
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i] = next_bullet;
            break;
        }
    }
}

extern void set_pixel(int8_t col, int8_t row); // from display.c

void move_x(int8_t direction)
{
    int8_t new_left_pos = ship.left_wing_index + direction;
    if (new_left_pos >= 0 && new_left_pos <= 5)
        ship.left_wing_index = (uint8_t)new_left_pos;
}

void move_y(int8_t direction)
{
    int8_t new_nose_pos = ship.nose_index + direction;
    if (new_nose_pos >= 1 && new_nose_pos <= 7)
        ship.nose_index = (uint8_t)new_nose_pos;
}

void draw_spaceship()
{
    for (int width = 0; width < ship_width; width++)
    {
        set_pixel(ship.left_wing_index + width, ship.nose_index - 1);
    }
    set_pixel(ship.left_wing_index + 1, ship.nose_index);

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            {
                set_pixel(bullets[i].x_pos, bullets[i].y_pos);
            }
        }
    }
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            for (int j = 0; j < asteroids[i].dimmension_count; j++)
            {
                set_pixel(asteroids[i].dimmensions[j].x_pos,
                          asteroids[i].dimmensions[j].y_pos);
            }
        }
    }
}

void bullet_task(void *pvParameters)
{

    BulletTaskContext ctx = {
        .bullets = bullets,
    };

    while (1)
    {
        if (bullet_fired)
        {
            shoot_bullet();
            bullet_fired = !bullet_fired;
        }
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (ctx.bullets[i].active)
            {
                if (ctx.bullets[i].y_pos <= ROW_COUNT - 2)
                {
                    ctx.bullets[i].y_pos += 1;
                    set_pixel(ctx.bullets[i].x_pos, ctx.bullets[i].y_pos);
                }
                else
                {
                    ctx.bullets[i].active = false;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
void generate_asteroid()
{
    Asteroid new_asteroid;
    new_asteroid.active = true;
    int8_t starting_col = rand() % (COL_COUNT - 1); // want room on the right for next asteroid column

    int8_t dimmension_count = 0;
    Point dimmensions[4];

    while (dimmension_count < 2)
    {
        dimmension_count = 0;

        int8_t random_val = rand() & 0x0F;
        for (int i = 0; i < 4; i++)
        {
            bool is_lit = (random_val >> i) & 1;
            if (is_lit)
            {
                dimmensions[dimmension_count++] = (Point){
                    .x_pos = starting_col + (i % 2),
                    .y_pos = ROW_COUNT - 1 + (i / 2), // for only allowing half of it to start in visible area
                };
                // dimmension_count += 1;
            }
        }
    }
    new_asteroid.dimmension_count = dimmension_count;
    for (int i = 0; i < dimmension_count; i++)
    {
        new_asteroid.dimmensions[i] = dimmensions[i];
    }
    bool stored = false;
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (!asteroids[i].active)
        {
            asteroids[i] = new_asteroid;
            stored = true;
            break;
        }
    }
    if (!stored)
    {
        printf("Warning: no free asteroid slot!\n");
    }
}

void asteroid_task(void *pvParameters)
{
    int tick = 0;

    while (1)
    {
        for (int i = 0; i < MAX_ASTEROIDS; i++)
        {
            if (asteroids[i].active)
            {
                for (int j = 0; j < asteroids[i].dimmension_count; j++)
                {
                
                    set_pixel(asteroids[i].dimmensions[j].x_pos,
                              asteroids[i].dimmensions[j].y_pos);
                    asteroids[i].dimmensions[j].y_pos -= 1;

                    if (asteroids[i].dimmensions[j].y_pos <= -2)
                    {
                        asteroids[i].active = false;
                    }
                }
            }
        }

        if (tick % 5 == 0)
        {
            generate_asteroid();
            printf("Generated asteroid\n");
        }

        tick++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}