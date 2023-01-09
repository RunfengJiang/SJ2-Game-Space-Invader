#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t curr_location[2];
  uint8_t width_height[2];
  bool is_collided;
} game_missile;