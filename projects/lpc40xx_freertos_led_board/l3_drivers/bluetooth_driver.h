#include "lpc_peripherals.h"
#include <stdbool.h>
#include <stdint.h>

void led_board__init_uart3(lpc_peripheral_e uart, uint16_t peripheral_clock,
                           uint32_t baud_rate);

bool bluetooth_send_data_uart3(char *output_data);
