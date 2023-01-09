#include "gpio_driver.h"
#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

void gpio_dir(uint8_t port_num, uint16_t pin_num, uint8_t type) {

  if (port_num == 0) {

    if (type == 0) {
      LPC_GPIO0->DIR |= (1 << pin_num);
    } else {
      LPC_GPIO0->DIR &= ~(1 << pin_num);
    }

  } else if (port_num == 1) {

    if (type == 0) {
      LPC_GPIO1->DIR |= (1 << pin_num);
    } else {
      LPC_GPIO1->DIR &= ~(1 << pin_num);
    }

  } else if (port_num == 2) {

    if (type == 0) {
      LPC_GPIO2->DIR |= (1 << pin_num);
    } else {
      LPC_GPIO2->DIR &= ~(1 << pin_num);
    }

  } else if (port_num == 3) {

    if (type == 0) {
      LPC_GPIO3->DIR |= (1 << pin_num);
    } else {
      LPC_GPIO3->DIR &= ~(1 << pin_num);
    }
  }
}

void gpio_set(uint8_t port_num, uint16_t pin_num, bool state) {

  if (port_num == 0) {

    if (state == true) {
      LPC_GPIO0->SET |= (1 << pin_num);
    } else {
      LPC_GPIO0->CLR |= (1 << pin_num);
    }

  } else if (port_num == 1) {

    if (state == true) {
      LPC_GPIO1->SET |= (1 << pin_num);
    } else {
      LPC_GPIO1->CLR |= (1 << pin_num);
    }

  } else if (port_num == 2) {

    if (state == true) {
      LPC_GPIO2->SET |= (1 << pin_num);
    } else {
      LPC_GPIO2->CLR |= (1 << pin_num);
    }

  } else if (port_num == 3) {

    if (state == true) {
      LPC_GPIO3->SET |= (1 << pin_num);
    } else {
      LPC_GPIO3->CLR |= (1 << pin_num);
    }
  }
}

bool gpio_get(uint8_t port_num, uint16_t pin_num) {

  if (port_num == 0) {

    if (LPC_GPIO0->PIN &= (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
  } else if (port_num == 1) {

    if (LPC_GPIO1->PIN &= (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
  } else if (port_num == 2) {

    if (LPC_GPIO2->PIN &= (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
  } else if (port_num == 3) {

    if (LPC_GPIO3->PIN &= (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
  }
}
