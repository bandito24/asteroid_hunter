#include "spaceship.h"

Bullet bullets[MAX_BULLETS] = {0};
static Asteroid asteroids[MAX_ASTEROIDS] = {0};

uint8_t ship_width = 3;
uint8_t ship_height = 2;
static Spaceship ship;
static uint32_t destroyed_asteroids = 0;
volatile static bool bullet_fired = false;
gameplay_state gameplay;

void IRAM_ATTR bullet_interrupt() { bullet_fired = true; }

void game_play_init() {

    reset_spaceship();
    ship.cannon_index = &ship.dimmensions[3];
    ship.left_wing_index = &ship.dimmensions[0];
    gameplay.level = 1;
    gameplay.idle = true;

    const gpio_config_t bullet_btn_conf = {.pin_bit_mask = 1ULL << bullet_gpio,
                                           .intr_type = GPIO_INTR_NEGEDGE,
                                           .pull_up_en = GPIO_PULLUP_ENABLE,
                                           .pull_down_en =
                                               GPIO_PULLDOWN_DISABLE,
                                           .mode = GPIO_MODE_INPUT};
    ESP_ERROR_CHECK(gpio_config(&bullet_btn_conf));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(bullet_gpio, bullet_interrupt, NULL));
}

void shoot_bullet() {
    Bullet next_bullet = {
        .x_pos = ship.cannon_index->x_pos,
        .y_pos = ship.cannon_index->y_pos,
        .active = true,
    };
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i] = next_bullet;
            break;
        }
    }
}

extern void set_pixel(int8_t col, int8_t row); // from display.c

void move_x(int8_t direction) {
    int8_t new_left_pos = ship.left_wing_index->x_pos + direction;
    if (new_left_pos >= -1 &&
        new_left_pos <=
            6) // letting the ships wings go off the screen to get corner
               // asteroids. set pixel has safeguard for no render
        draw_spaceship(new_left_pos, ship.cannon_index->y_pos);
}

void move_y(int8_t direction) {
    int8_t new_nose_pos = ship.cannon_index->y_pos + direction;
    if (new_nose_pos >= 1 && new_nose_pos <= 7)
        // ship.nose_index = (uint8_t)new_nose_pos;
        draw_spaceship(ship.left_wing_index->x_pos, new_nose_pos);
}

static uint64_t counter = 0;

void gameplay_wait() {
    counter += 1;
    if (gameplay.has_lost) {
        display_loss();
    } else if (gameplay.has_won) {
        display_win();
    } else if (gameplay.idle) {
        display_level(gameplay.level);
    }
    if (counter > 40) {
        counter = 0;
        if (gameplay.has_lost || gameplay.has_won) {
            gameplay.has_lost = false;
            gameplay.has_won = false;
        } else {
            gameplay.idle = false;
        }
    }
}
void gameplay_continue() {

    for (int dim = 0; dim < 4; dim++) {
        set_pixel(ship.dimmensions[dim].x_pos, ship.dimmensions[dim].y_pos);
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            {
                set_pixel(bullets[i].x_pos, bullets[i].y_pos);
            }
        }
    }
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            for (int j = 0; j < asteroids[i].dimmension_count; j++) {
                set_pixel(asteroids[i].dimmensions[j].x_pos,
                          asteroids[i].dimmensions[j].y_pos);
            }
        }
    }
}

void draw_gameplay() {
    if (gameplay.idle) {
        gameplay_wait();
    } else {
        gameplay_continue();
    }
}
bool asteroid_destroy(int8_t x_pos, int8_t y_pos) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        for (int j = 0; j < asteroids[i].dimmension_count; j++) {
            if (asteroids[i].dimmensions[j].x_pos == x_pos &&
                asteroids[i].dimmensions[j].y_pos == y_pos && asteroids[i].active) {
                asteroids[i].active = false;
                destroyed_asteroids += 1;
                if (destroyed_asteroids >= ASTEROID_LEVEL_THRESHOLD &&
                    gameplay.level < MAX_LEVELS) {
                    activate_win();
                }
                return true;
            }
        }
    }
    return false;
}

void bullet_task(void *pvParameters) {

    while (1) {
        if (gameplay.idle) {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (bullet_fired) {
            shoot_bullet();
            bullet_fired = !bullet_fired;
        }
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                if (bullets[i].y_pos <= ROW_COUNT - 2) {
                    bullets[i].y_pos += 1;
                    if (asteroid_destroy(bullets[i].x_pos, bullets[i].y_pos)) {
                        bullets[i].active = false;
                    } else {
                        set_pixel(bullets[i].x_pos, bullets[i].y_pos);
                    }
                } else {
                    bullets[i].active = false;
                }
            }
        }

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                for (int j = 0; j < 4; j++) {
                    for (int k = 0; k < asteroids[i].dimmension_count; k++) {

                        if (ship.dimmensions[j].x_pos ==
                                asteroids[i].dimmensions[k].x_pos &&
                            ship.dimmensions[j].y_pos ==
                                asteroids[i].dimmensions[k].y_pos) {
                            activate_loss();
                        }
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void generate_asteroid() {
    Asteroid new_asteroid;
    new_asteroid.active = true;
    int8_t starting_col =
        rand() %
        (COL_COUNT - 1); // want room on the right for next asteroid column

    int8_t dimmension_count = 0;
    Point dimmensions[4];

    while (dimmension_count < 2) {
        dimmension_count = 0;

        int8_t random_val = rand() & 0x0F;
        for (int i = 0; i < 4; i++) {
            bool is_lit = (random_val >> i) & 1;
            if (is_lit) {
                dimmensions[dimmension_count++] = (Point){
                    .x_pos = starting_col + (i % 2),
                    .y_pos =
                        ROW_COUNT - 1 + (i / 2), // for only allowing half of it
                                                 // to start in visible area
                };
            }
        }
    }
    new_asteroid.dimmension_count = dimmension_count;
    for (int i = 0; i < dimmension_count; i++) {
        new_asteroid.dimmensions[i] = dimmensions[i];
    }
    bool stored = false;
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active) {
            asteroids[i] = new_asteroid;
            stored = true;
            break;
        }
    }
    if (!stored) {
        printf("Warning: no free asteroid slot!\n");
    }
}

void end_level() {
    gameplay.idle = true;
    reset_spaceship();
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].active = false;
    }
    destroyed_asteroids = 0;
}

void activate_loss() {
    end_level();
    gameplay.level = 1;
    gameplay.has_lost = true;
}
void activate_win() {
    end_level();
    gameplay.level += 1;
    gameplay.has_won = true;
}

void draw_spaceship(int8_t left_wing_index, int8_t nose_index) {
    for (int i = 0; i < ship_width; i++) {
        ship.dimmensions[i].x_pos = left_wing_index + i;
        ship.dimmensions[i].y_pos = nose_index - 1;
    }
    ship.dimmensions[3].x_pos = left_wing_index + 1;
    ship.dimmensions[3].y_pos = nose_index;
}

void reset_spaceship() { draw_spaceship(0, 1); }

const int decrement_step = (STARTING_ASTEROID_RATE_MS - 100) /
                           MAX_LEVELS; // want no faster than every 100ms

void asteroid_task(void *pvParameters) {
    int tick = 0;

    while (1) {

        if (gameplay.idle) {

            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                for (int j = 0; j < asteroids[i].dimmension_count; j++) {

                    set_pixel(asteroids[i].dimmensions[j].x_pos,
                              asteroids[i].dimmensions[j].y_pos);
                    asteroids[i].dimmensions[j].y_pos -= 1;

                    if (asteroids[i].dimmensions[j].y_pos <= -1) {
                        activate_loss();
                    }
                }
            }
        }

        if (tick % 5 == 0) {
            generate_asteroid();
            printf("Generated asteroid\n");
        }
        uint16_t curr_speed =
            STARTING_ASTEROID_RATE_MS - (decrement_step * gameplay.level);

        tick++;
        vTaskDelay(pdMS_TO_TICKS(curr_speed));
    }
}
