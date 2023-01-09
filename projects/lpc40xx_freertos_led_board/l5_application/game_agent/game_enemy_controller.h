#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef struct {
  uint8_t curr_location[2];
  uint8_t width_height[2];
  bool is_alive;
} game_enemy;

game_enemy game_env__enemy_init_random_at_right(void);