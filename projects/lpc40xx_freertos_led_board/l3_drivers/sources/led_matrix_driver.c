#include "led_matrix_driver.h"
#include "FreeRTOS.h"
#include "led_matrix_io.h"
#include "task.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static uint8_t matrixbuff[32][64];

static bool is_upper_matrix(uint8_t row, uint8_t col, uint8_t color);
static bool is_lower_matrix(uint8_t row, uint8_t col, uint8_t color);

static bool is_upper_matrix(uint8_t row, uint8_t col, uint8_t color) {
  return (matrixbuff[row][col] & color && (row < 16));
}

static bool is_lower_matrix(uint8_t row, uint8_t col, uint8_t color) {
  return (matrixbuff[row][col] & color && (row > 15));
}

void overwrite_pattern_to_screen(uint8_t **pattern) {
  display_clear();
  memcpy(matrixbuff, pattern, (1 * 32 * 64));
}

void append_pattern_to_screen(uint8_t *pattern, uint8_t *location,
                              uint8_t *width_height) {
  static int tempX = 0;
  static int tempY = 0;
  for (int i = location[0]; i < location[0] + width_height[1]; i++) {
    for (int j = location[1]; j < location[1] + width_height[0]; j++) {
      tempX = i - location[0];
      tempY = j - location[1];
      matrixbuff[i][j] += pattern[tempX * width_height[0] + tempY];
    }
  }
}

void clear_pattern_on_screen(uint8_t *location, uint8_t *width_height) {
  if (location[1] + width_height[0] > 64 ||
      location[0] + width_height[1] > 32) {
    return; // clear pattern unsuccessful.
  }
  for (int i = location[0]; i < location[0] + width_height[1]; i++) {
    for (int j = location[1]; j < location[1] + width_height[0]; j++) {
      matrixbuff[i][j] = 0;
    }
  }
}

void display_init(void) { memset(matrixbuff, 0, 32 * 64); }

void display_update(void) {
  const uint8_t red_num = 0x4;
  const uint8_t green_num = 0x2;
  const uint8_t blue_num = 0x1;

  gpio_ss *gpp_table = get_gpio_table();

  for (int row = 0; row < 32; row++) {
    for (int col = 0; col < 64; col++) {
      if (is_upper_matrix(row, col, blue_num)) {
        led_matrix_rgb_control(gpp_table[B1_e].gpp, true);
      } else {
        led_matrix_rgb_control(gpp_table[B1_e].gpp, false);
      }
      if (is_upper_matrix(row, col, green_num)) {
        led_matrix_rgb_control(gpp_table[G1_e].gpp, true);
      } else {
        led_matrix_rgb_control(gpp_table[G1_e].gpp, false);
      }
      if (is_upper_matrix(row, col, red_num)) {
        led_matrix_rgb_control(gpp_table[R1_e].gpp, true);
      } else {
        led_matrix_rgb_control(gpp_table[R1_e].gpp, false);
      }
      if (is_lower_matrix(row, col, blue_num)) {
        led_matrix_rgb_control(gpp_table[B2_e].gpp, true);
      } else {
        led_matrix_rgb_control(gpp_table[B2_e].gpp, false);
      }
      if (is_lower_matrix(row, col, green_num)) {
        led_matrix_rgb_control(gpp_table[G2_e].gpp, true);
      } else {
        led_matrix_rgb_control(gpp_table[G2_e].gpp, false);
      }
      if (is_lower_matrix(row, col, red_num)) {
        led_matrix_rgb_control(gpp_table[R2_e].gpp, true);
      } else {
        led_matrix_rgb_control(gpp_table[R2_e].gpp, false);
      }

      led_matrix_clk_latch_oe_control(gpp_table[CLK_e].gpp, true);
      led_matrix_clk_latch_oe_control(gpp_table[CLK_e].gpp, false);
    }

    led_matrix_clk_latch_oe_control(gpp_table[OE_e].gpp, true);
    led_matrix_clk_latch_oe_control(gpp_table[LAT_e].gpp, true);

    led_matrix_row_control(gpp_table[A_e].gpp, false);
    led_matrix_row_control(gpp_table[B_e].gpp, false);
    led_matrix_row_control(gpp_table[C_e].gpp, false);
    led_matrix_row_control(gpp_table[D_e].gpp, false);

    if (row & 0x1) {
      led_matrix_row_control(gpp_table[A_e].gpp, true);
    }
    if (row & 0x2) {
      led_matrix_row_control(gpp_table[B_e].gpp, true);
    }
    if (row & 0x4) {
      led_matrix_row_control(gpp_table[C_e].gpp, true);
    }
    if (row & 0x8) {
      led_matrix_row_control(gpp_table[D_e].gpp, true);
    }

    led_matrix_clk_latch_oe_control(gpp_table[LAT_e].gpp, false);
    led_matrix_clk_latch_oe_control(gpp_table[OE_e].gpp, false);
  }
}

void display_updating(void *p) {
  while (1) {
    display_update();
    // BJ, 2022-11-15: if display fresh rate as 1 tick per ms, the edge will
    // become brighter than the rest of the screen vTaskDelay(1);
  }
}

void draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
  matrixbuff[x][y] = color;
}

void display_clear(void) {
  for (int row = 0; row < 32; row++) {
    for (int col = 0; col < 64; col++) {
      matrixbuff[row][col] = 0;
    }
  }
}

uint8_t **get_led_matrix_handler(void) { return matrixbuff; }

int return_given_area_matrix_sum(uint8_t *location, uint8_t *width_height) {
  int out = 0;
  for (int i = location[0]; i < location[0] + width_height[1]; i++) {
    for (int j = location[1]; j < location[1] + width_height[0]; j++) {
      out += matrixbuff[i][j];
    }
  }
  return out;
}