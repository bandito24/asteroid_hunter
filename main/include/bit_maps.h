#pragma once
#include <stdint.h>

const uint8_t level_bit_maps[10][8] = {
    { 0x00, 0x00, 0x21, 0x7F, 0x01, 0x00, 0x00, 0x00 }, // "1"
    { 0x00, 0x00, 0x23, 0x45, 0x49, 0x31, 0x00, 0x00 }, // "2"
    { 0x00, 0x00, 0x22, 0x49, 0x49, 0x36, 0x00, 0x00 }, // "3"
    { 0x00, 0x00, 0x0E, 0x32, 0x7F, 0x02, 0x00, 0x00 }, // "4"
    { 0x00, 0x00, 0x79, 0x49, 0x49, 0x46, 0x00, 0x00 }, // "5"
    { 0x00, 0x00, 0x3E, 0x49, 0x49, 0x26, 0x00, 0x00 }, // "6"
    { 0x00, 0x00, 0x60, 0x47, 0x48, 0x70, 0x00, 0x00 }, // "7"
    { 0x00, 0x00, 0x36, 0x49, 0x49, 0x36, 0x00, 0x00 }, // "8"
    { 0x00, 0x00, 0x32, 0x49, 0x49, 0x3E, 0x00, 0x00 }, // "9"
    { 0x00, 0x00, 0x3E, 0x41, 0x41, 0x3E, 0x00, 0x00 }  // "0"
};


uint8_t loss_pattern[8] = {
    0b10000001, // Row 0: X shape
    0b01000010, // Row 1
    0b00100100, // Row 2
    0b00011000, // Row 3
    0b00011000, // Row 4
    0b00100100, // Row 5
    0b01000010, // Row 6
    0b10000001  // Row 7
};
uint8_t win_pattern[8] = {
    0b00001000, // Row 0
    0b00000100, // Row 1
    0b00000010, // Row 2
    0b00000100, // Row 3
    0b00001000, // Row 4
    0b00010000, // Row 5
    0b00100000, // Row 6
    0b01000000  // Row 7
};

