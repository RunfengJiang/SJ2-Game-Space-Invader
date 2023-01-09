#include "analog_joystick_driver.h"
#include "FreeRTOS.h"
#include "adc.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "task.h"
#include <stdint.h>

typedef enum {
  channel_x,
  channel_y,
} adc_channel_ce;

typedef struct {
  char *name;
  uint8_t portNum;
  uint8_t pinNum;
  gpio_s gpp;
} adc_channel_cs;

static adc_channel_cs adc_channel_table[] = {
    {.name = "x_channel", .portNum = 0, .pinNum = 25},
    {.name = "y_channel", .portNum = 1, .pinNum = 30},
};

static volatile uint32_t *gpio__get_iocon(gpio_s gpio);
static void adc_channel_2_io_init(uint8_t port, uint8_t pinNum);
static void adc_channel_4_io_init(uint8_t port, uint8_t pinNum);
static int x_reading = 0;
static int y_reading = 0;

static volatile uint32_t *gpio__get_iocon(gpio_s gpio) {
  volatile uint32_t *IOCON_base = (volatile uint32_t *)LPC_IOCON;

  // Each port is offset by thirty-two 32-bit registers
  volatile uint32_t *port_offset = IOCON_base + (32 * gpio.port_number);

  // Each pin configuration is a single 32-bit
  volatile uint32_t *pin_iocon = (port_offset + gpio.pin_number);

  return pin_iocon;
}

static void adc_channel_2_io_init(uint8_t port, uint8_t pinNum) {
  // set channels depends on the port, pins
  int adc2_function_select = GPIO__FUNCTION_1;
  adc_channel_table[channel_x].gpp =
      gpio__construct_with_function(port, pinNum, adc2_function_select);
  volatile uint32_t *pin_iocon =
      gpio__get_iocon(adc_channel_table[channel_x].gpp);
  // Enable analog mode and disable pull up/down resistors
  *pin_iocon &= ~((0b11 << 3) | (1 << 7));
}

static void adc_channel_4_io_init(uint8_t port, uint8_t pinNum) {
  // set channels depends on the port, pins
  int adc4_function_select = GPIO__FUNCTION_3;
  (void)gpio__construct_with_function(port, pinNum, adc4_function_select);
  adc_channel_table[channel_y].gpp =
      gpio__construct_with_function(port, pinNum, adc4_function_select);
  volatile uint32_t *pin_iocon =
      gpio__get_iocon(adc_channel_table[channel_y].gpp);
  // Enable analog mode and disable pull up/down resistors
  *pin_iocon &= ~((0b11 << 3) | (1 << 7));
}

void joystick_init(void) {
  adc__initialize();

  adc__enable_burst_mode(ADC__CHANNEL_2);
  adc__enable_burst_mode(ADC__CHANNEL_4);

  adc_channel_2_io_init(adc_channel_table[channel_x].portNum,
                        adc_channel_table[channel_x].pinNum);
  adc_channel_4_io_init(adc_channel_table[channel_y].portNum,
                        adc_channel_table[channel_y].pinNum);
}

void joystick_running(void *p) {
  while (1) {
    x_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_2);
    y_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_4);
    vTaskDelay(50);
  }
}

int *get_joystick_input_handler(void) {
  static int x_and_y_readings[2];
  x_and_y_readings[0] = x_reading;
  x_and_y_readings[1] = y_reading;
  return x_and_y_readings;
}