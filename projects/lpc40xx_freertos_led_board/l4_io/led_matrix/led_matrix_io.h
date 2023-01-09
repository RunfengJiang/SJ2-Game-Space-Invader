#pragma once

#include "gpio.h"
#include <stdbool.h>

typedef enum {
  R1_e,
  G1_e,
  B1_e,
  R2_e,
  G2_e,
  B2_e,
  A_e,
  B_e,
  C_e,
  D_e,
  CLK_e,
  LAT_e,
  OE_e,
} gpp_e;

typedef struct {
  const char *name;
  gpio_s gpp;
  const uint8_t portNum;
  const uint8_t pinNum;
} gpio_ss;

void led_matrix_io_init(void);
void led_matrix_row_control(gpio_s rowNum, bool state);
void led_matrix_rgb_control(gpio_s rgb, bool state);
void led_matrix_clk_latch_oe_control(gpio_s gpio_name, bool state);
gpio_ss *get_gpio_table(void);