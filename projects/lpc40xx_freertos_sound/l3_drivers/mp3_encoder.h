#pragma once

#include "stdbool.h"
#include "stdint.h"

void uart3_init(void);
bool uart3_get(char *input_byte);
bool uart3_put(char output_byte);
bool send_data_to_MP3(char cmd, uint8_t msb, uint8_t lsb);
bool stop_song(void);

volatile int received;