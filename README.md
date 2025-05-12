# Asteroid Hunter - ESP32 Game for LED Matrix

Welcome to **Asteroid Hunter**, a classic take on the Asteroids game, designed to run on an ESP32 with an LED matrix display. The game puts you in control of a spaceship, where the objective is to clear 9 levels by destroying incoming asteroids. Your goal is to avoid letting any asteroids hit your ship or reach the bottom of the screen without being destroyed.

## Game Description
In **Asteroid Hunter**, you control a spaceship using a clickable joystick. The game features 9 progressively challenging levels. As asteroids descend from the top of the screen, you must shoot them before they reach the bottom. If an asteroid hits the ship, or if an asteroid reaches the bottom without being destroyed, you lose.

### Objective:
- Clear all 9 levels by destroying the asteroids.
- Avoid asteroids colliding with the ship.
- Prevent any asteroids from reaching the bottom of the screen.

## Hardware Requirements
To run **Asteroid Hunter**, you'll need the following hardware:

- **ESP32** microcontroller
- **LED Matrix** (for displaying the game)
- **Clickable Joystick** (for controlling the ship)
- **Shift Register** (recommended for controlling the LED matrix)
  
### Wiring up the LED Matrix:
Getting this game up and running will require connecting the LED matrix properly to your ESP32. If you’re new to working with LED matrices, this will be a great challenge and learning experience. We highly recommend using a shift register to control the LED matrix, as it makes wiring much easier and reduces the number of GPIO pins required.

## Configuration
You can easily clone the source code and modify the game parameters by editing `config.h`. In this file, you can customize:

- LED matrix size (adjust the rows and columns as needed)
- GPIO pin numbers for the joystick and LED matrix connections

### Example Configuration (`config.h`):
```c
// Define LED Matrix Size
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8

// Define GPIO pins for the LED Matrix and Joystick
#define LED_MATRIX_PIN_1  5
#define LED_MATRIX_PIN_2  6
#define JOYSTICK_PIN_UP   7
#define JOYSTICK_PIN_DOWN 8
