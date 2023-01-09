#include "game_enemy_controller.h"
#include <math.h>

game_enemy game_env__enemy_init_random_at_right(void) {
  static uint8_t random_generate_spot_x[6] = {0, 5, 10, 15, 20, 25};
  game_enemy temp = {
      .curr_location[0] = random_generate_spot_x[rand() % 6],
      .curr_location[1] = 64 - 6,
      .is_alive = true,
      .width_height[0] = 6,
      .width_height[1] = 5,
  };
  return temp;
}