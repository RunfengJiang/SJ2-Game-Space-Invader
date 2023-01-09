#include <stdio.h>
#include <string.h>

#include "unity.h"

void NVIC_SystemReset(void) {}

/**
 * Generate "Mocks" for these files
 */
#include "Mockperiodic_callbacks.h"
#include "Mocktask.h"

/**
 * Include the source we wish to test
 * There are two options:
 *   1. Include 'periodic_scheduler.h'
 *      In this case, we can test the API as a black box, but not get into the
 * details (or static functions)
 *   2. Include 'periodic_scheduler.c'
 *      In this case, our test file here can see all of the internals of the
 * file as the whole *.c file is sort of "inlined" right above our source code
 * here
 */
#include "periodic_scheduler.c"

void setUp(void) {}

void tearDown(void) {}

void test_periodic_scheduler__initialize(void) {
  const uint32_t stack_size = 1024;
  const TaskHandle_t returned_task_handle = NULL;

  xTaskCreateStatic_ExpectAndReturn(periodic_scheduler__1000Hz_task, "1000Hz",
                                    stack_size, NULL, PRIORITY_PERIODIC_1000HZ,
                                    NULL, NULL, returned_task_handle);
  xTaskCreateStatic_IgnoreArg_puxStackBuffer();
  xTaskCreateStatic_IgnoreArg_pxTaskBuffer();

  xTaskCreateStatic_ExpectAndReturn(periodic_scheduler__1Hz_task, "1Hz",
                                    stack_size, NULL, PRIORITY_PERIODIC_1HZ,
                                    NULL, NULL, returned_task_handle);
  xTaskCreateStatic_IgnoreArg_puxStackBuffer();
  xTaskCreateStatic_IgnoreArg_pxTaskBuffer();

  xTaskCreateStatic_ExpectAndReturn(periodic_scheduler__10Hz_task, "10Hz",
                                    stack_size, NULL, PRIORITY_PERIODIC_10HZ,
                                    NULL, NULL, returned_task_handle);
  xTaskCreateStatic_IgnoreArg_puxStackBuffer();
  xTaskCreateStatic_IgnoreArg_pxTaskBuffer();

  xTaskCreateStatic_ExpectAndReturn(periodic_scheduler__100Hz_task, "100Hz",
                                    stack_size, NULL, PRIORITY_PERIODIC_100HZ,
                                    NULL, NULL, returned_task_handle);
  xTaskCreateStatic_IgnoreArg_puxStackBuffer();
  xTaskCreateStatic_IgnoreArg_pxTaskBuffer();

  xTaskCreateStatic_ExpectAndReturn(
      periodic_scheduler__task_monitor, "Hz_wdt", stack_size / 4, NULL,
      PRIORITY_PERIODIC_MONITOR, NULL, NULL, returned_task_handle);
  xTaskCreateStatic_IgnoreArg_puxStackBuffer();
  xTaskCreateStatic_IgnoreArg_pxTaskBuffer();

  periodic_callbacks__initialize_Expect();
  periodic_scheduler__initialize_with_1khz();
}