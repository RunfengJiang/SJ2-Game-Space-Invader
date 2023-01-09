#include "game_player_controller.h"
#include "analog_joystick_driver.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

const static uint8_t speed = 1;
const static uint8_t screen_width = 64;
const static uint8_t screen_height = 32;
const static uint8_t left_edge = 6;

static player_moves curr_move;
static player_moves decode_x_y_reading_from_joystick(int *xy_readings);

static bool is_stay_center(int *xy_readings);
static bool is_move_up(int *xy_readings);
static bool is_move_down(int *xy_readings);
static bool is_move_left(int *xy_readings);
static bool is_move_right(int *xy_readings);

static bool is_move_left_up(int *xy_readings);
static bool is_move_left_down(int *xy_readings);
static bool is_move_right_up(int *xy_readings);
static bool is_move_right_down(int *xy_readings);

static uint8_t *agent_shift_up(uint8_t *location);
static uint8_t *agent_shift_down(uint8_t *location,
                                 uint8_t *agent_width_height);
static uint8_t *agent_shift_left(uint8_t *location);
static uint8_t *agent_shift_right(uint8_t *location,
                                  uint8_t *agent_height_width);

static uint8_t *agent_shift_left_up(uint8_t *location);
static uint8_t *agent_shift_left_down(uint8_t *location,
                                      uint8_t *agent_height_width);
static uint8_t *agent_shift_right_up(uint8_t *location,
                                     uint8_t *agent_height_width);
static uint8_t *agent_shift_right_down(uint8_t *location,
                                       uint8_t *agent_height_width);

static bool is_stay_center(int *xy_readings) {
  return (abs(xy_readings[0] - 2000) <= 200 &&
          abs(xy_readings[1] - 2000) <= 200);
}

static bool is_move_up(int *xy_readings) {
  return (abs(xy_readings[0] - 2000) <= 200 && xy_readings[1] >= 2500);
}

static bool is_move_down(int *xy_readings) {
  return (abs(xy_readings[0] - 2000) <= 200 && xy_readings[1] <= 1500);
}

static bool is_move_left(int *xy_readings) {
  return (xy_readings[0] <= 1500 && abs(xy_readings[1] - 2000) <= 200);
}

static bool is_move_right(int *xy_readings) {
  return (xy_readings[0] >= 2500 && abs(xy_readings[1] - 2000) <= 200);
}

static bool is_move_left_up(int *xy_readings) {
  return (xy_readings[0] <= 1500 && xy_readings[1] >= 2500);
}

static bool is_move_left_down(int *xy_readings) {
  return (xy_readings[0] <= 1500 && xy_readings[1] <= 1500);
}

static bool is_move_right_up(int *xy_readings) {
  return (xy_readings[0] >= 2500 && xy_readings[1] >= 2500);
}

static bool is_move_right_down(int *xy_readings) {
  return (xy_readings[0] >= 2500 && xy_readings[1] <= 1500);
}

static player_moves decode_x_y_reading_from_joystick(int *xy_readings) {
  player_moves temp_move = 0;
  if (is_stay_center(xy_readings)) {
    temp_move = stay_center;
  } else if (is_move_up(xy_readings)) {
    temp_move = move_up;
  } else if (is_move_down(xy_readings)) {
    temp_move = move_down;
  } else if (is_move_left(xy_readings)) {
    temp_move = move_left;
  } else if (is_move_right(xy_readings)) {
    temp_move = move_right;
  } else if (is_move_left_up(xy_readings)) {
    temp_move = move_left_up;
  } else if (is_move_left_down(xy_readings)) {
    temp_move = move_left_down;
  } else if (is_move_right_up(xy_readings)) {
    temp_move = move_right_up;
  } else if (is_move_right_down(xy_readings)) {
    temp_move = move_right_down;
  }
  return temp_move;
}

static uint8_t *agent_shift_up(uint8_t *location) {
  static uint8_t out_location[2];
  out_location[0] = location[0] - (1 * speed);
  out_location[1] = location[1];
  if (out_location[0] <= 0 || out_location[0] >= 255) {
    out_location[0] = 0;
  }
  return out_location;
}

static uint8_t *agent_shift_down(uint8_t *location,
                                 uint8_t *agent_width_height) {
  static uint8_t out_location[2];
  out_location[0] = location[0] + (1 * speed);
  out_location[1] = location[1];
  if (out_location[0] >= screen_height - agent_width_height[1] ||
      out_location[0] >= 255) {
    out_location[0] = screen_height - agent_width_height[1];
  }
  return out_location;
}

static uint8_t *agent_shift_left(uint8_t *location) {
  static uint8_t out_location[2];
  out_location[0] = location[0];
  out_location[1] = location[1] - (1 * speed);
  if (out_location[1] <= left_edge || out_location[1] >= 255) {
    out_location[1] = left_edge;
  }
  return out_location;
}

static uint8_t *agent_shift_right(uint8_t *location,
                                  uint8_t *agent_width_height) {
  static uint8_t out_location[2];
  out_location[0] = location[0];
  out_location[1] = location[1] + (1 * speed);
  if (out_location[1] >= screen_width - agent_width_height[0] ||
      out_location[1] >= 255) {
    out_location[1] = screen_width - agent_width_height[0];
  }
  return out_location;
}

static uint8_t *agent_shift_left_up(uint8_t *location) {
  static uint8_t out_location[2];
  out_location[0] = location[0] - (1 * speed);
  out_location[1] = location[1] - (1 * speed);
  if (out_location[0] <= 0 || out_location[0] >= 255) {
    out_location[0] = 0;
  }
  if (out_location[1] <= left_edge || out_location[1] >= 255) {
    out_location[1] = left_edge;
  }
  return out_location;
}

static uint8_t *agent_shift_left_down(uint8_t *location,
                                      uint8_t *agent_width_height) {
  static uint8_t out_location[2];
  out_location[0] = location[0] + (1 * speed);
  out_location[1] = location[1] - (1 * speed);
  if (out_location[0] >= screen_height - agent_width_height[1] ||
      out_location[0] >= 255) {
    out_location[0] = screen_height - agent_width_height[1];
  }
  if (out_location[1] <= left_edge || out_location[1] >= 255) {
    out_location[1] = left_edge;
  }
  return out_location;
}

static uint8_t *agent_shift_right_up(uint8_t *location,
                                     uint8_t *agent_width_height) {
  static uint8_t out_location[2];
  out_location[0] = location[0] - (1 * speed);
  out_location[1] = location[1] + (1 * speed);
  if (out_location[0] <= 0 || out_location[0] >= 255) {
    out_location[0] = 0;
  }
  if (out_location[1] >= screen_width - agent_width_height[0] ||
      out_location[1] >= 255) {
    out_location[1] = screen_width - agent_width_height[0];
  }
  return out_location;
}

static uint8_t *agent_shift_right_down(uint8_t *location,
                                       uint8_t *agent_width_height) {
  static uint8_t out_location[2];
  out_location[0] = location[0] + (1 * speed);
  out_location[1] = location[1] + (1 * speed);
  if (out_location[0] >= screen_height - agent_width_height[1] ||
      out_location[0] >= 255) {
    out_location[0] = screen_height - agent_width_height[1];
  }
  if (out_location[1] >= screen_width - agent_width_height[0] ||
      out_location[1] >= 255) {
    out_location[1] = screen_width - agent_width_height[0];
  }
  return out_location;
}

uint8_t *game_agent__update_location(uint8_t *location,
                                     uint8_t *agent_width_height) {
  uint8_t *temp_location = location;
  switch (curr_move) {
  case stay_center:
    temp_location = location;
    break;

  case move_up:
    temp_location = agent_shift_up(location);
    break;

  case move_down:
    temp_location = agent_shift_down(location, agent_width_height);
    break;

  case move_left:
    temp_location = agent_shift_left(location);
    break;

  case move_right:
    temp_location = agent_shift_right(location, agent_width_height);
    break;

  case move_left_up:
    temp_location = agent_shift_left_up(location);
    break;

  case move_left_down:
    temp_location = agent_shift_left_down(location, agent_width_height);
    break;

  case move_right_up:
    temp_location = agent_shift_right_up(location, agent_width_height);
    break;

  case move_right_down:
    temp_location = agent_shift_right_down(location, agent_width_height);
    break;
  }

  return temp_location;
}

player_moves game_agent__return_joystick_move(void) {
  int *xy_readings = get_joystick_input_handler();
  curr_move = decode_x_y_reading_from_joystick(xy_readings);
  return curr_move;
}

game_player game_agent__agent_init(void) {
  game_player player = {
      .curr_location[0] = 10,
      .curr_location[1] = 10,
      .width_height[0] = 6,
      .width_height[1] = 5,
      .is_alive = true,
      .heart_count = 3,
      .invincible_time_when_got_hit = 20,
  };
  return player;
}