#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  stay_center,
  move_up,
  move_down,
  move_left,
  move_right,
  move_left_up,
  move_left_down,
  move_right_up,
  move_right_down,
} player_moves;

typedef struct {
  uint8_t curr_location[2];
  uint8_t width_height[2];
  uint8_t heart_count;
  bool is_alive;
  int invincible_time_when_got_hit;
} game_player;

uint8_t *game_agent__update_location(uint8_t *location,
                                     uint8_t *agent_width_height);
player_moves game_agent__return_joystick_move(void);
game_player game_agent__agent_init(void);
