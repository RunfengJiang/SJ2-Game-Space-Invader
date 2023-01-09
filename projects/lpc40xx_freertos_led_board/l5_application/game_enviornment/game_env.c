#include "game_env.h"
#include "button_driver.h"
#include "delay.h"
#include "game_enemy_controller.h"
#include "game_missile.h"
#include "game_player_controller.h"
#include "led_matrix_driver.h"
#include "led_matrix_patterns.h"
#include "mp3_encoder.h"
#include "score_system.h"
#include "semphr.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LONG_TIME portMAX_DELAY

static bool is_player_hit = false;
static bool is_backGround_playing = false;

static SemaphoreHandle_t green_button_signal;
static SemaphoreHandle_t collision_check_mutex = NULL;

static int enemy_move_speed = 100;
static int enemy_spawn_time = 50;
static int enemy_spawn_timer = 0;

static game_player space_ship;
static uint8_t heart_init_location[2] = {0, 0};
static uint8_t heart_shape_width_height[2] = {4, 5};

static game_enemy right_enemy_list[10];
static int right_enemy_remain_counts = 0;

static game_missile game_missile_list[20];
static uint8_t in_game_missile_count = 0;
static uint8_t missile_speed = 2;

static game_states prev_state = prep_state;
static game_states curr_state = start_up;

static void player_game_start_screen(game_player a_agent);
static void player_update_heart_on_screen(void);
static void is_player_dead_change_state(void);
static void game_env__update_player_location(void);

static void right_enemy_shift_left_in_loop(game_enemy *enemy, uint8_t index);
static void generate_missile_from_player(void);
static void missile_shift_right(game_missile *a_missile, uint8_t index);

static void remove_missile_from_list_when_collide(uint8_t index);
static void remove_enemy_from_list_when_collide(uint8_t index);
static void remove_missile_from_list(uint8_t index);
static void remove_enemy_from_list(uint8_t index);

static void empty_enemy_list();
static void empty_missile_list();

static void reset_all_game_parameters(void);
static void resume_game_mechanism(void);
static int adjust_enemy_spawn_time(int curr_score);
static int adjust_enemy_speed(int curr_score);
// mp3 commands
static void play_backGround_music_inLoop(void);
static void play_missile_fire_sound(void);
static void play_gameOver_music(void);
static void play_spaceShipExplodedMusic(void);
static void play_invaderKilledMusic(void);
static void play_123readyMusic(void);

static void player_game_start_screen(game_player a_agent) {
  display_clear();
  for (int i = 0; i < a_agent.heart_count; i++) {
    append_pattern_to_screen(heart, heart_init_location,
                             heart_shape_width_height);
    heart_init_location[0] += 5;
  }
  for (int j = 0; j < 32; j++) {
    draw_pixel(j, 5, 7); // draw white line to seperate
  }
  score__reset();
  append_pattern_to_screen(good_guy1, a_agent.curr_location,
                           a_agent.width_height);
}

static void game_env__update_player_location(void) {
  player_moves agent_move;
  agent_move = game_agent__return_joystick_move();

  if (agent_move != stay_center) {
    clear_pattern_on_screen(space_ship.curr_location, space_ship.width_height);
    static uint8_t *updated_location;
    updated_location = game_agent__update_location(space_ship.curr_location,
                                                   space_ship.width_height);
    space_ship.curr_location[0] = updated_location[0];
    space_ship.curr_location[1] = updated_location[1];

    if (is_player_hit) {
      vTaskDelay(100);
    }

    append_pattern_to_screen(good_guy1, space_ship.curr_location,
                             space_ship.width_height);
  }

  if (agent_move == stay_center && is_player_hit) {
    clear_pattern_on_screen(space_ship.curr_location, space_ship.width_height);
    static uint8_t *updated_location;
    updated_location = game_agent__update_location(space_ship.curr_location,
                                                   space_ship.width_height);
    space_ship.curr_location[0] = updated_location[0];
    space_ship.curr_location[1] = updated_location[1];

    vTaskDelay(100);

    append_pattern_to_screen(good_guy1, space_ship.curr_location,
                             space_ship.width_height);
  }

  if (space_ship.invincible_time_when_got_hit >= 20) {
    is_player_hit = false;
    // vTaskResume(button);
    int player_area = return_given_area_matrix_sum(space_ship.curr_location,
                                                   space_ship.width_height);
    if (player_area > 100) { // when player got hit
      is_player_hit = true;
      space_ship.heart_count -= 1;
      player_update_heart_on_screen();
      space_ship.invincible_time_when_got_hit = 0;
      play_spaceShipExplodedMusic();
    }
  } else {
    space_ship.invincible_time_when_got_hit++;
  }
}

static void player_update_heart_on_screen(void) {
  heart_init_location[0] = 0;
  uint8_t threeHeartWidthHeight[2] = {4, 15};
  clear_pattern_on_screen(heart_init_location, threeHeartWidthHeight);
  for (int i = 0; i < space_ship.heart_count; i++) {
    append_pattern_to_screen(heart, heart_init_location,
                             heart_shape_width_height);
    heart_init_location[0] += 5;
  }
}

static void right_enemy_shift_left_in_loop(game_enemy *enemy, uint8_t index) {
  clear_pattern_on_screen(enemy->curr_location, enemy->width_height);
  enemy->curr_location[1] -= 1;
  if (enemy->curr_location[1] <= 5) {
    enemy->curr_location[1] = 64 - 6;
    score__decrease(2);
  }
  append_pattern_to_screen(bad_guy1, enemy->curr_location, enemy->width_height);
  int enemy_area =
      return_given_area_matrix_sum(enemy->curr_location, enemy->width_height);
  if (enemy_area > 32) {
    enemy->is_alive = false;
    score__increase(2);
  }
}

static void generate_missile_from_player(void) {
  game_missile new_missile;
  uint8_t curr_missile_x = space_ship.curr_location[0] + 2;
  uint8_t curr_missile_y = space_ship.curr_location[1] + 6 + 1;
  new_missile.width_height[0] = 3;
  new_missile.width_height[1] = 1;
  new_missile.curr_location[0] = curr_missile_x;
  new_missile.curr_location[1] = curr_missile_y;
  new_missile.is_collided = false;
  append_pattern_to_screen(missile, new_missile.curr_location,
                           new_missile.width_height);
  if (in_game_missile_count < 10) {
    game_missile_list[in_game_missile_count] = new_missile;
    ++in_game_missile_count;
  }
}

static void missile_shift_right(game_missile *a_missile, uint8_t index) {
  if (a_missile->curr_location[1] >= 64 - 3) {
    a_missile->is_collided = true;
    remove_missile_from_list(index);
  } else {
    clear_pattern_on_screen(a_missile->curr_location, a_missile->width_height);
    a_missile->curr_location[1] += 1;
    append_pattern_to_screen(missile, a_missile->curr_location,
                             a_missile->width_height);

    int missile_area = return_given_area_matrix_sum(a_missile->curr_location,
                                                    a_missile->width_height);
    if (missile_area > 9) {
      a_missile->is_collided = true;
    }
  }
}

static void remove_missile_from_list_when_collide(uint8_t index) {
  if (game_missile_list[index].is_collided) {
    clear_pattern_on_screen(game_missile_list[index].curr_location,
                            game_missile_list[index].width_height);
    for (int i = index; i < in_game_missile_count - 1; i++) {
      game_missile_list[i] = game_missile_list[i + 1];
    }
    --in_game_missile_count;
  }
}

static void remove_enemy_from_list_when_collide(uint8_t index) {
  if (!right_enemy_list[index].is_alive) {
    clear_pattern_on_screen(right_enemy_list[index].curr_location,
                            right_enemy_list[index].width_height);
    for (int i = index; i < right_enemy_remain_counts - 1; i++) {
      right_enemy_list[i] = right_enemy_list[i + 1];
    }
    --right_enemy_remain_counts;
  }
}

static void remove_missile_from_list(uint8_t index) {
  clear_pattern_on_screen(game_missile_list[index].curr_location,
                          game_missile_list[index].width_height);
  for (int i = index; i < in_game_missile_count - 1; i++) {
    game_missile_list[i] = game_missile_list[i + 1];
  }
  --in_game_missile_count;
}

static void remove_enemy_from_list(uint8_t index) {
  clear_pattern_on_screen(right_enemy_list[index].curr_location,
                          right_enemy_list[index].width_height);
  for (int i = index; i < right_enemy_remain_counts - 1; i++) {
    right_enemy_list[i] = right_enemy_list[i + 1];
  }
  --right_enemy_remain_counts;
}

static void is_player_dead_change_state(void) {
  if (space_ship.heart_count <= 0) {
    curr_state = game_over;
  }
}

static void empty_enemy_list() {
  for (int i = 0; i < right_enemy_remain_counts; i++) {
    remove_enemy_from_list(i);
  }
}

static void empty_missile_list() {
  for (int i = 0; i < in_game_missile_count; i++) {
    remove_missile_from_list(i);
  }
}

static void reset_all_game_parameters(void) {
  enemy_spawn_timer = 0;
  right_enemy_remain_counts = 0;
  in_game_missile_count = 0;
  space_ship.invincible_time_when_got_hit = 20;
  vTaskSuspend(enemy_moving);
  vTaskSuspend(missile_moving);
  vTaskSuspend(score_running);
  vTaskSuspend(level);
}

static void resume_game_mechanism(void) {
  vTaskResume(enemy_moving);
  vTaskResume(missile_moving);
  vTaskResume(score_running);
  vTaskResume(level);
}

static void play_backGround_music_inLoop(void) {
  if (!is_backGround_playing) {
    is_backGround_playing = true;
    mp3__cyclePlay_folderName(0x01);
  }
}

static void play_missile_fire_sound(void) {
  mp3__play_folderName_songName(0x02, 0x02);
}

static void play_gameOver_music(void) {
  if (!is_backGround_playing) {
    is_backGround_playing = true;
    mp3__play_folderName_songName(0x03, 0x03);
  }
}

static void play_spaceShipExplodedMusic(void) {
  mp3__play_folderName_songName(0x04, 0x01);
}

static void play_invaderKilledMusic(void) {
  mp3__play_folderName_songName(0x04, 0x02);
}

static void play_123readyMusic(void) {
  mp3__play_folderName_songName(0x04, 0x03);
}

static int adjust_enemy_spawn_time(int curr_score) {
  int out = 50;
  if (curr_score > 4 && curr_score < 100) {
    out -= 40;
  } else if (curr_score > 100 && curr_score < 200) {
    out -= 45;
  } else if (curr_score > 200 && curr_score < 400) {
    out -= 50;
  } else if (curr_score > 400 && curr_score < 2000) {
    out -= 55;
  }
  return out;
}

static int adjust_enemy_speed(int curr_score) {
  int out = 100;
  if (curr_score > 4 && curr_score < 100) {
    out -= 40;
  } else if (curr_score > 100 && curr_score < 200) {
    out -= 45;
  } else if (curr_score > 200 && curr_score < 400) {
    out -= 50;
  } else if (curr_score > 400 && curr_score < 2000) {
    out -= 55;
  }
  return out;
}

void game_env__enemy_task(void *p) {
  while (1) {
    switch (curr_state) {
    case start_up:
      break;

    case game_running:
      if (xSemaphoreTake(collision_check_mutex, LONG_TIME)) {
        if (enemy_spawn_timer >= enemy_spawn_time) {
          if (right_enemy_remain_counts < 10) {
            game_enemy enemy_temp = game_env__enemy_init_random_at_right();
            right_enemy_list[right_enemy_remain_counts] = enemy_temp;
            right_enemy_remain_counts++;
          }
          enemy_spawn_timer = 0;
        }
        enemy_spawn_timer++;
        for (int i = 0; i < right_enemy_remain_counts; i++) {
          if (right_enemy_list[i].is_alive) {
            right_enemy_shift_left_in_loop(&(right_enemy_list[i]), i);
          } else {
            remove_enemy_from_list(i);
            play_invaderKilledMusic();
          }
        }
        xSemaphoreGive(collision_check_mutex);
      }
      break;

    case game_over:
      break;
    }
    vTaskDelay(enemy_move_speed);
  }
}

void game_env__player_task(void *p) {
  collision_check_mutex = xSemaphoreCreateMutex();
  while (1) {
    switch (curr_state) {
    case start_up:
      if (prev_state != start_up) {
        space_ship = game_agent__agent_init();
        reset_all_game_parameters();
        overwrite_pattern_to_screen(start_up_screen1);
        prev_state = curr_state;
      }
      break;

    case game_running:
      if (prev_state != game_running) {
        resume_game_mechanism();
        player_game_start_screen(space_ship);
        prev_state = curr_state;
      }
      if (xSemaphoreTake(collision_check_mutex, LONG_TIME)) {
        game_env__update_player_location();
        is_player_dead_change_state();
        xSemaphoreGive(collision_check_mutex);
      }
      break;

    case game_over:
      break;
    }
    vTaskDelay(50);
  }
}

void game_env__button_task(void *p) {
  while (1) {
    switch (curr_state) {
    case start_up:
      play_backGround_music_inLoop();
      green_button_signal = return_green_button_binarySemphr_handle();
      if (button_return_press_status()) {
        play_123readyMusic();
        overwrite_pattern_to_screen(countDown_number3);
        delay__ms(1.3 * 1000);
        overwrite_pattern_to_screen(countDown_number2);
        delay__ms(1.3 * 1000);
        overwrite_pattern_to_screen(countDown_number1);
        delay__ms(1.3 * 1000);
        curr_state = game_running;
        button_reset_press_status();
      }
      break;

    case game_running:
      if (is_backGround_playing) {
        mp3__pause();
        is_backGround_playing = false;
      }
      if (button_return_press_status()) {
        if (!is_player_hit) {
          play_missile_fire_sound();
          generate_missile_from_player();
        }
        button_reset_press_status();
      }
      break;

    case game_over:
      play_gameOver_music();
      if (prev_state != game_over) {
        reset_all_game_parameters();
        overwrite_pattern_to_screen(game_over_screen1);
        prev_state = curr_state;
        button_reset_press_status();
        vTaskDelay(3000);
      } else if (button_return_press_status()) {
        is_backGround_playing = false;
        curr_state = start_up;
        button_reset_press_status();
      }
      break;
    }
    vTaskDelay(20);
  }
}

void game_env__missile_task(void *p) {
  while (1) {
    switch (curr_state) {
    case start_up:
      break;

    case game_running:
      if (xSemaphoreTake(collision_check_mutex, LONG_TIME)) {
        for (int i = 0; i < in_game_missile_count; i++) {
          if (!game_missile_list[i].is_collided) {
            missile_shift_right(&(game_missile_list[i]), i);
          } else {
            remove_missile_from_list(i);
          }
        }
        xSemaphoreGive(collision_check_mutex);
      }
      break;

    case game_over:
      break;
    }
    vTaskDelay(missile_speed * 10);
  }
}

void game_env__raise_difficult_level(void *p) {
  while (1) {
    int get_score = score__return_score();
    enemy_spawn_time = adjust_enemy_spawn_time(get_score);
    enemy_move_speed = adjust_enemy_speed(get_score);
    vTaskDelay(50);
  }
}