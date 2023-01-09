#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

#include "gpio_driver.h"
#include "led_matrix.h"

#include "lpc_peripherals.h"

#include "mp3_encoder.h"
#include "portmacro.h"
#include "semphr.h"
#include "uart.h"
#include <string.h>

extern int i2c_data;

int main(void) {

  // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
  // Change '#if 0' to '#if 1' and vice versa to try it out
#if 0
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
  // UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
#endif

  // mp3
  gpio_dir(2, 2, 1);
  LPC_IOCON->P2_2 |= 0x00;

  gpio_dir(2, 0, 1);
  LPC_IOCON->P2_0 |= 0x00;

  gpio_dir(1, 28, 1);
  LPC_IOCON->P1_28 |= 0x00;

  gpio_dir(1, 29, 1);
  LPC_IOCON->P1_29 |= 0x00;

  uart3_init();
  received = 0;
  // Play first song 7E FF 06 03 00 00 01 EF
  for (int i = 1; i < 3; i++) {
    for (int j = 1; j < 4; j++) {
      received = 0;
      sendDataToMP3(0x0F, i, j);
      while (received != 1)
        ;

      // stop playback
      received = 0;
      sendDataToMP3(0x16, 0x00, 0x00);
      while (received != 1)
        ;
    }
  }

  printf("=====1======");
  return 0;
}
