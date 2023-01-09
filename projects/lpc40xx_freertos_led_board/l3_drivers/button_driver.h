#pragma once
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdbool.h>

void button_init_as_interrupt_from_isr(void);
void button_init_as_normal_gpio(void);
void green_button_pressed(void);
void button_pressed_rtosTask(void *p);
bool button_return_press_status(void);
void button_reset_press_status(void);

SemaphoreHandle_t return_green_button_binarySemphr_handle(void);