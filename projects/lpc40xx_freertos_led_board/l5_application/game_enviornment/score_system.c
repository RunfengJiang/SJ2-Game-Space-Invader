#include "score_system.h"
#include "FreeRTOS.h"
#include "led_matrix_driver.h"
#include "task.h"
#include <stdint.h>

static uint8_t score_number_width_height[2] = {5, 3};
static uint8_t placeHolderCount = 4;
static int score_record = 0;

static void draw_score_by_index(uint8_t *score, uint8_t index);
static void update_screen_by_score(uint8_t scoreDigit, uint8_t index);

typedef struct {
  uint8_t score_init_location[2];
} score_place_holder;

static score_place_holder scorePlaceHolder_table[] = {
    {.score_init_location[0] = 29, .score_init_location[1] = 0},
    {.score_init_location[0] = 25, .score_init_location[1] = 0},
    {.score_init_location[0] = 21, .score_init_location[1] = 0},
    {.score_init_location[0] = 17, .score_init_location[1] = 0},
};

static uint8_t score_number0[3][5] = {
    4, 4, 4, 4, 4, 4, 0, 0, 0, 4, 4, 4, 4, 4, 4,
};

static uint8_t score_number1[3][5] = {
    4, 0, 0, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0,
};

static uint8_t score_number2[3][5] = {
    4, 4, 4, 0, 4, 4, 0, 4, 0, 4, 4, 0, 4, 4, 4,
};

static uint8_t score_number3[3][5] = {
    4, 0, 4, 0, 4, 4, 0, 4, 0, 4, 4, 4, 4, 4, 4,
};

static uint8_t score_number4[3][5] = {
    0, 0, 4, 4, 4, 0, 0, 4, 0, 0, 4, 4, 4, 4, 4,
};

static uint8_t score_number5[3][5] = {
    4, 0, 4, 4, 4, 4, 0, 4, 0, 4, 4, 4, 4, 0, 4,
};

static uint8_t score_number6[3][5] = {
    4, 4, 4, 4, 4, 4, 0, 4, 0, 4, 4, 4, 4, 0, 4,
};

static uint8_t score_number7[3][5] = {
    0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4,
};

static uint8_t score_number8[3][5] = {
    4, 4, 4, 4, 4, 4, 0, 4, 0, 4, 4, 4, 4, 4, 4,
};

static uint8_t score_number9[3][5] = {
    0, 0, 4, 4, 4, 0, 0, 4, 0, 4, 4, 4, 4, 4, 4,
};

static void draw_score_by_index(uint8_t *score, uint8_t index) {
  clear_pattern_on_screen(scorePlaceHolder_table[index].score_init_location,
                          score_number_width_height);
  append_pattern_to_screen(score,
                           scorePlaceHolder_table[index].score_init_location,
                           score_number_width_height);
}

static void update_screen_by_score(uint8_t scoreDigit, uint8_t index) {
  switch (scoreDigit) {
  case 0:
    draw_score_by_index(score_number0, index);
    break;
  case 1:
    draw_score_by_index(score_number1, index);
    break;
  case 2:
    draw_score_by_index(score_number2, index);
    break;
  case 3:
    draw_score_by_index(score_number3, index);
    break;
  case 4:
    draw_score_by_index(score_number4, index);
    break;
  case 5:
    draw_score_by_index(score_number5, index);
    break;
  case 6:
    draw_score_by_index(score_number6, index);
    break;
  case 7:
    draw_score_by_index(score_number7, index);
    break;
  case 8:
    draw_score_by_index(score_number8, index);
    break;
  case 9:
    draw_score_by_index(score_number9, index);
    break;
  }
}

void score__reset(void) {
  score_record = 0;
  for (int i = 0; i < placeHolderCount; i++) {
    draw_score_by_index(score_number0, i);
  }
}

void score__increase(int number) { score_record += number; }
void score__decrease(int number) { score_record -= number; }

void score__update(void) {
  if (score_record > 9999) {
    score_record = 9999;
  }
  if (score_record < 0) {
    score_record = 0;
  }
  int temp, lastDigit;
  uint8_t index = 0;
  temp = score_record;
  while (index < 4) {
    lastDigit = temp % 10;
    update_screen_by_score(lastDigit, index);
    temp = temp / 10;
    index += 1;
  }
}

void score__updating(void *p) {
  while (1) {
    score__update();
    vTaskDelay(100);
  }
}

int score__return_score(void) { return score_record; }