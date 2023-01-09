#include "l3_drivers/mp3_encoder.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "uart.h"
#include <stdio.h>

static void mp3__u3_isr(void);

static void mp3__u3_isr(void) {

  uint8_t RDR = 1;

  char data = 'A';

  if (LPC_UART3->LSR & RDR) {
    data = LPC_UART3->RBR;
  }

  if (data == 0xEF) {
    received = 1;
  }
}

void mp3__uart3_init(lpc_peripheral_e uart, uint16_t peripheral_clock,
                     uint32_t baud_rate) {

  lpc_peripheral__turn_on_power_to(uart);

  int uart3_function_select = GPIO__FUNCTION_2;

  (void)gpio__construct_with_function(4, 28, uart3_function_select);
  (void)gpio__construct_with_function(4, 29, uart3_function_select);

  // set DLAB bit to access DLL and DLM
  LPC_UART3->LCR |= 1 << 7;

  // selects word length = 8 bit
  LPC_UART3->LCR |= 0x3;

  // set clock divisor
  const uint16_t divider_16_bit =
      peripheral_clock * 1000 * 1000 / (16 * baud_rate);
  LPC_UART3->DLL = (divider_16_bit & 0xFF);
  LPC_UART3->DLM = ((divider_16_bit >> 8) & 0xFF);

  // clears DLAB bit
  LPC_UART3->LCR &= ~(1 << 7);

  // enable interrupt
  LPC_UART3->IER |= 0x1;

  // enable NVIC
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART3, mp3__u3_isr,
                                   "INT_callback");
}

bool mp3__uart3_get(char *input_byte) {

  uint8_t RDR = 1;

  if (LPC_UART3->LSR & RDR) {
    *input_byte = LPC_UART3->RBR;
    return true;
  } else {
    return false;
  }
}

bool mp3__uart3_put(char output_byte) {

  uint8_t THRE = 5;

  if (LPC_UART3->LSR & (1 << THRE)) {
    LPC_UART3->THR = output_byte;
    while (!(LPC_UART3->LSR & (1 << THRE)))
      ;
  }

  return true;
}

void mp3__pause(void) { mp3__send_data_to_MP3(0x0E, 0x00, 0x00); }

void mp3__resume(void) { mp3__send_data_to_MP3(0x0D, 0x00, 0x00); }

bool mp3__send_data_to_MP3(char cmd, uint8_t msb, uint8_t lsb) {
  // Play song by number 7E FF 06 03 01 00 XX EF
  // XX is the number of the song
  // Play song by location 7E FF 06 0F 00 QQ XX EF
  // QQ is folder number
  // XX is song number

  mp3__uart3_put(0x7E); // start
  mp3__uart3_put(0xFF); // version, always 0xFF
  mp3__uart3_put(0x06); // length of data, always 0x06
  mp3__uart3_put(cmd);  // command
  mp3__uart3_put(0x01); // feedback
  mp3__uart3_put(msb);  // dataHi
  mp3__uart3_put(lsb);  // dataLo
  mp3__uart3_put(0xEF); // stop

  return true;
}

void mp3__device_init(void) {
  mp3__send_data_to_MP3(0x09, 0x00, 0x02); // select TF card device
  mp3__send_data_to_MP3(0x06, 0x00, 0x1E); // set volume to 30
}

void mp3__cyclePlay_folderName(uint8_t folderName) {
  mp3__send_data_to_MP3(0x17, folderName, 0x02);
}

void mp3__play_folderName_songName(uint8_t folderName, uint8_t songName) {
  mp3__send_data_to_MP3(0x0F, folderName, songName);
}