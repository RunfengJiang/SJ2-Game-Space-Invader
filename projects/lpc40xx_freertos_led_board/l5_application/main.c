#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "button_driver.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

#include "analog_joystick_driver.h"
#include "bluetooth_driver.h"
#include "game_env.h"
#include "led_matrix_driver.h"
#include "led_matrix_io.h"
#include "lpc_peripherals.h"
#include "mp3_encoder.h"
#include "score_system.h"

#include <stdio.h>

// 'static' to make these functions 'private' to this file
#if 0
static void create_blinky_tasks(void);
static void create_uart_task(void);
static void blink_task(void *params);
static void uart_task(void *params);
#endif

void simple_task__mp3(void *p) {
  received = 0;
  while (1) {
    if (mp3__send_data_to_MP3(0x0F, 0x01, 0x01)) {
      fprintf(stderr, "music starts\n");
    }
    vTaskDelay(500);
  }
}

int main(void) {
#if 0
  create_blinky_tasks();
  create_uart_task();
#endif

  // If you have the ESP32 wifi module soldered on the board, you can try
  // uncommenting this code See esp32/README.md for more details uart3_init();
  // // Also include:  uart3_init.h xTaskCreate(esp32_tcp_hello_world_task,
  // "uart3", 1000, NULL, PRIORITY_LOW, NULL); // Include esp32_task.h
  sj2_cli__init();

  led_matrix_io_init();

  display_init();

  joystick_init();

  // button_init_as_normal_gpio();

  button_init_as_interrupt_from_isr();

  mp3__uart3_init(LPC_PERIPHERAL__UART3, 96, 9600);

  mp3__device_init();

  xTaskCreate(display_updating, "disp_update", 4096 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);

  xTaskCreate(joystick_running, "adc_task", 4096 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);

  xTaskCreate(game_env__player_task, "player", 4096 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);

  xTaskCreate(game_env__button_task, "gameButton", 1024 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, &button);

  xTaskCreate(game_env__enemy_task, "enemy", 4096 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, &enemy_moving);

  xTaskCreate(game_env__missile_task, "missile", 4096 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, &missile_moving);

  xTaskCreate(button_pressed_rtosTask, "buttonDetect", 1024 / sizeof(void *),
              NULL, PRIORITY_MEDIUM, NULL);

  xTaskCreate(score__updating, "score", 4096 / sizeof(void *), NULL,
              PRIORITY_MEDIUM, &score_running);

  xTaskCreate(game_env__raise_difficult_level, "level", 4096 / sizeof(void *),
              NULL, PRIORITY_MEDIUM, &level);

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler

  return 0;
}
#if 0
static void create_blinky_tasks(void) {
  /**
   * Use '#if (1)' if you wish to observe how two tasks can blink LEDs
   * Use '#if (0)' if you wish to use the 'periodic_scheduler.h' that will spawn 4 periodic tasks, one for each LED
   */
#if (1)
  // These variables should not go out of scope because the 'blink_task' will reference this memory
  static gpio_s led0, led1;

  // If you wish to avoid malloc, use xTaskCreateStatic() in place of xTaskCreate()
  static StackType_t led0_task_stack[512 / sizeof(StackType_t)];
  static StackType_t led1_task_stack[512 / sizeof(StackType_t)];
  static StaticTask_t led0_task_struct;
  static StaticTask_t led1_task_struct;

  led0 = board_io__get_led0();
  led1 = board_io__get_led1();

  xTaskCreateStatic(blink_task, "led0", ARRAY_SIZE(led0_task_stack), (void *)&led0, PRIORITY_LOW, led0_task_stack,
                    &led0_task_struct);
  xTaskCreateStatic(blink_task, "led1", ARRAY_SIZE(led1_task_stack), (void *)&led1, PRIORITY_LOW, led1_task_stack,
                    &led1_task_struct);
#else
  periodic_scheduler__initialize();
  UNUSED(blink_task);
#endif
}

static void create_uart_task(void) {
  // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
  // Change '#if (0)' to '#if (1)' and vice versa to try it out
#if (0)
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
  UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
#endif
}

static void blink_task(void *params) {
  const gpio_s led = *((gpio_s *)params); // Parameter was input while calling xTaskCreate()

  // Warning: This task starts with very minimal stack, so do not use printf() API here to avoid stack overflow
  while (true) {
    gpio__toggle(led);
    vTaskDelay(500);
  }
}

// This sends periodic messages over printf() which uses system_calls.c to send them to UART0
static void uart_task(void *params) {
  TickType_t previous_tick = 0;
  TickType_t ticks = 0;

  while (true) {
    // This loop will repeat at precise task delay, even if the logic below takes variable amount of ticks
    vTaskDelayUntil(&previous_tick, 2000);

    /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire output will be fully
     * sent out before this function returns. See system_calls.c for actual implementation.
     *
     * Use this style print for:
     *  - Interrupts because you cannot use printf() inside an ISR
     *    This is because regular printf() leads down to xQueueSend() that might block
     *    but you cannot block inside an ISR hence the system might crash
     *  - During debugging in case system crashes before all output of printf() is sent
     */
    ticks = xTaskGetTickCount();
    fprintf(stderr, "%u: This is a polled version of printf used for debugging ... finished in", (unsigned)ticks);
    fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

    /* This deposits data to an outgoing queue and doesn't block the CPU
     * Data will be sent later, but this function would return earlier
     */
    ticks = xTaskGetTickCount();
    printf("This is a more efficient printf ... finished in");
    printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
  }
}
#endif