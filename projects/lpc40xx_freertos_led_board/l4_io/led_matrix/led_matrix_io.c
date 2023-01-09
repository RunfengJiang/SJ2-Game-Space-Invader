#include "led_matrix_io.h"
#include "pwm1.h"
#include <stdint.h>

static gpio_ss gpio_table[] = {
    {.name = "R1", .portNum = 2, .pinNum = 0},
    {.name = "G1", .portNum = 2, .pinNum = 2},
    {.name = "B1", .portNum = 2, .pinNum = 5},
    {.name = "R2", .portNum = 2, .pinNum = 1},
    {.name = "G2", .portNum = 2, .pinNum = 4},
    {.name = "B2", .portNum = 2, .pinNum = 6},
    {.name = "A", .portNum = 0, .pinNum = 6},
    {.name = "B", .portNum = 0, .pinNum = 26},
    {.name = "C", .portNum = 0, .pinNum = 16},
    {.name = "D", .portNum = 0, .pinNum = 22},
    {.name = "CLK", .portNum = 2, .pinNum = 9},
    {.name = "LAT", .portNum = 1, .pinNum = 20},
    {.name = "OE", .portNum = 1, .pinNum = 28},
};

static void rgb_pio_init(void);
static void row_sel_pio_init(void);
static void latch_pio_init(void);
static void clk_pio_init(void);
static void oe_pio_init(void);

static void rgb_pio_init(void) {
  gpio_table[R1_e].gpp = gpio__construct_as_output(gpio_table[R1_e].portNum,
                                                   gpio_table[R1_e].pinNum);
  gpio_table[G1_e].gpp = gpio__construct_as_output(gpio_table[G1_e].portNum,
                                                   gpio_table[G1_e].pinNum);
  gpio_table[B1_e].gpp = gpio__construct_as_output(gpio_table[B1_e].portNum,
                                                   gpio_table[B1_e].pinNum);

  gpio_table[R2_e].gpp = gpio__construct_as_output(gpio_table[R2_e].portNum,
                                                   gpio_table[R2_e].pinNum);
  gpio_table[G2_e].gpp = gpio__construct_as_output(gpio_table[G2_e].portNum,
                                                   gpio_table[G2_e].pinNum);
  gpio_table[B2_e].gpp = gpio__construct_as_output(gpio_table[B2_e].portNum,
                                                   gpio_table[B2_e].pinNum);
}

static void row_sel_pio_init(void) {
  gpio_table[A_e].gpp = gpio__construct_as_output(gpio_table[A_e].portNum,
                                                  gpio_table[A_e].pinNum);
  gpio_table[B_e].gpp = gpio__construct_as_output(gpio_table[B_e].portNum,
                                                  gpio_table[B_e].pinNum);
  gpio_table[C_e].gpp = gpio__construct_as_output(gpio_table[C_e].portNum,
                                                  gpio_table[C_e].pinNum);
  gpio_table[D_e].gpp = gpio__construct_as_output(gpio_table[D_e].portNum,
                                                  gpio_table[D_e].pinNum);
}

static void clk_pio_init(void) {
  gpio_table[CLK_e].gpp = //
      gpio__construct_as_output(gpio_table[CLK_e].portNum,
                                gpio_table[CLK_e].pinNum);
}

static void latch_pio_init(void) {
  gpio_table[LAT_e].gpp = //
      gpio__construct_as_output(gpio_table[LAT_e].portNum,
                                gpio_table[LAT_e].pinNum);
}

static void oe_pio_init(void) {
  gpio_table[OE_e].gpp = //
      gpio__construct_as_output(gpio_table[OE_e].portNum,
                                gpio_table[OE_e].pinNum);
}

gpio_ss *get_gpio_table(void) { return gpio_table; }

void led_matrix_row_control(gpio_s rowNum, bool state) {
  if (state) {
    gpio__set(rowNum);
  } else {
    gpio__reset(rowNum);
  }
}

void led_matrix_rgb_control(gpio_s rgb, bool state) {
  if (state) {
    gpio__set(rgb);
  } else {
    gpio__reset(rgb);
  }
}

void led_matrix_clk_latch_oe_control(gpio_s gpio_name, bool state) {
  if (state) {
    gpio__set(gpio_name);
  } else {
    gpio__reset(gpio_name);
  }
}

void led_matrix_io_init(void) {
  rgb_pio_init();

  row_sel_pio_init();

  clk_pio_init();

  latch_pio_init();

  oe_pio_init();

  led_matrix_clk_latch_oe_control(gpio_table[OE_e].gpp, true);
}
