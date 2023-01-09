#pragma once

#include "stdbool.h"
#include "stdint.h"

void uart3_init(void);
bool sendDataToMP3(char cmd, uint8_t msb, uint8_t lsb);
bool uart3_get(char *input_byte);
bool uart3_put(char output_byte);

volatile int received;
