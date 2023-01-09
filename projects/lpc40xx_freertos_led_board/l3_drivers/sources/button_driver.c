#include "button_driver.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

static gpio_s green_button;
static SemaphoreHandle_t green_button_signal;
static uint8_t green_button_portPin[2] = {0, 17};
static bool is_button_pressed = false;

void button_init_as_interrupt_from_isr(void) {
  green_button_signal = xSemaphoreCreateBinary();
  green_button = gpio__construct_as_input(green_button_portPin[0],
                                          green_button_portPin[1]);
  LPC_GPIOINT->IO0IntEnR |=
      (1 << green_button_portPin[1]); // Enable gpio interrup falling edge
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, green_button_pressed,
                                   NULL);
}

void button_init_as_normal_gpio(void) {
  green_button = gpio__construct_as_input(green_button_portPin[0],
                                          green_button_portPin[1]);
}

void green_button_pressed(void) {
  xSemaphoreGiveFromISR(green_button_signal, NULL);
  LPC_GPIOINT->IO0IntClr |= (1 << green_button_portPin[1]);
}

SemaphoreHandle_t return_green_button_binarySemphr_handle(void) {
  return green_button_signal;
}

void button_pressed_rtosTask(void *p) {
  while (1) {
    vTaskDelay(50);
    if (gpio__get(green_button)) {
      vTaskDelay(50);
      is_button_pressed = true;
    }
    vTaskDelay(50); // button debounce
  }
}

bool button_return_press_status(void) { return is_button_pressed; }

void button_reset_press_status(void) { is_button_pressed = false; }