#pragma once

#include "lpc_peripherals.h"
#include <stdbool.h>
#include <stdint.h>

void mp3__uart3_init(lpc_peripheral_e uart, uint16_t peripheral_clock,
                     uint32_t baud_rate);
bool mp3__uart3_get(char *input_byte);
bool mp3__uart3_put(char output_byte);
bool mp3__send_data_to_MP3(char cmd, uint8_t msb, uint8_t lsb);
void mp3__pause(void);
void mp3__resume(void);
void mp3__device_init(void);
void mp3__cyclePlay_folderName(uint8_t folderName);
void mp3__play_folderName_songName(uint8_t folderName, uint8_t songName);

volatile int received;
