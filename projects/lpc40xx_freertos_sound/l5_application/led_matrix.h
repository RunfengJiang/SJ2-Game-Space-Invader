#pragma once

#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

void init_led_matrix();
void led_matrix_control(int type, bool state);
void updateDisplay(void);
int8_t updatePixel(uint8_t x, uint8_t y, uint8_t color);
void drawSpaceship(uint8_t x, uint8_t y, uint8_t color);
void clearDisplay(void);
void drawEnemy(uint8_t x, uint8_t y, uint8_t color);
// void firing_start_position(uint8_t x, uint8_t y, uint8_t color);

enum { R1, G1, B1, R2, G2, B2, A, B, C, D, CLK, LAT, OE };

uint8_t matrixbuff[32][64];

#endif