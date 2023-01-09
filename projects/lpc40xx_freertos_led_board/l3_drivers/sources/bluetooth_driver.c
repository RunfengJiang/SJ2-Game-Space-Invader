#include "bluetooth_driver.h"
#include "gpio.h"
#include "lpc40xx.h"

typedef enum {
  TX3,
  RX3,
} bluetooth_channel;

typedef struct {
  char *name;
  uint8_t portNum;
  uint8_t pinNum;
  gpio_s gpp;
} bluetooth_channel_cs;

static bluetooth_channel_cs bluetooth_channel_table[] = {
    {.name = "TX3", .portNum = 4, .pinNum = 28},
    {.name = "RX3", .portNum = 4, .pinNum = 29},
};

void led_board__init_uart3(lpc_peripheral_e uart, uint16_t peripheral_clock,
                           uint32_t baud_rate) {
  lpc_peripheral__turn_on_power_to(uart); // power_on UART

  int uart3_function_select = GPIO__FUNCTION_2;
  bluetooth_channel_table[RX3].gpp = gpio__construct_with_function(
      bluetooth_channel_table[RX3].portNum, bluetooth_channel_table[RX3].pinNum,
      uart3_function_select);

  bluetooth_channel_table[TX3].gpp = gpio__construct_with_function(
      bluetooth_channel_table[TX3].portNum, bluetooth_channel_table[TX3].pinNum,
      uart3_function_select);

  const uint8_t div_latch_access_bit = 7;
  uint8_t word_length = 3; // 8-bit UART
  const uint16_t divider_16_bit =
      peripheral_clock * 1000 * 1000 / (16 * baud_rate);

  LPC_UART3->LCR |= (1 << div_latch_access_bit);
  LPC_UART3->LCR |= (word_length << 0); // set char length
  LPC_UART3->DLM = (divider_16_bit >> 8) & 0xFF;
  LPC_UART3->DLL = divider_16_bit & 0xFF;
  LPC_UART3->LCR &= ~(1 << div_latch_access_bit);
}

bool bluetooth_send_data_uart3(char *output_data) {
  if (LPC_UART3->LSR & (1 << 5)) {
    LPC_UART3->THR = *output_data;
    return true;
  } else {
    return false;
  }
}