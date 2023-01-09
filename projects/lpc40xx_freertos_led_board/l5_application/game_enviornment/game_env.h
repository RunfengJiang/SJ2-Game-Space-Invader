#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

TaskHandle_t enemy_moving;
TaskHandle_t missile_moving;
TaskHandle_t score_running;
TaskHandle_t level;
TaskHandle_t button;

typedef enum {
  prep_state,
  start_up,
  game_running,
  game_over,
} game_states;

void game_env__player_task(void *p);
void game_env__enemy_task(void *p);
void game_env__button_task(void *p);
void game_env__missile_task(void *p);
void game_env__collision_check(void *p);
void game_env__raise_difficult_level(void *p);