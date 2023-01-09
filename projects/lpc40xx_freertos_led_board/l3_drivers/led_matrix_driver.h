#pragma once
#include <stdint.h>

void display_update(void);
void display_updating(void *p);
void display_clear(void);
void display_init(void);
void overwrite_pattern_to_screen(uint8_t **pattern);
void append_pattern_to_screen(uint8_t *pattern, uint8_t *location,
                              uint8_t *width_height);
void clear_pattern_on_screen(uint8_t *location, uint8_t *width_height);
void draw_pixel(uint8_t x, uint8_t y, uint8_t color);
uint8_t **get_led_matrix_handler(void);
int return_given_area_matrix_sum(uint8_t *location, uint8_t *width_height);